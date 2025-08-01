// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/OutputDeviceNull.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "MyMatineeCameraShake.h"

ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	DetectRange = 10000.f;
	AttackRange = 1500.f;
	TrackToRange = 400.F;

	isBoss = true;
	AttackCoolTime = OriginAttackCoolTime;
	TurnCoolTime = OriginAttackCoolTime;
	BackAttackCoolTime = OriginAttackCoolTime;
	SuddenAttackCoolTime = OriginAttackCoolTime;

	MyDamage = 15.f;
	MaxHP = 120.f;
	HP = MaxHP;
}

void ABossEnemy::Tick(float DeltaTime)
{
	RunCoolTime(DeltaTime);
	TickParabola(DeltaTime);
	RotationArroundToPlayer();

	if (isAttacking) 
		return;

	CalculateDistFromPlayer();
	CheckBackAttack();
	CheckSuddenAttack();
	Turn();

}

void ABossEnemy::RunCoolTime(float DeltaTime)
{
	TurnCoolTime -= DeltaTime;
	AttackCoolTime -= DeltaTime;
	BackAttackCoolTime -= DeltaTime;
	SuddenAttackCoolTime -= DeltaTime;
}

void ABossEnemy::CalculateDistFromPlayer()
{
	StartLocation = GetActorLocation();
	TargetLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	Distance = (TargetLocation - StartLocation).Size();
}

void ABossEnemy::CheckSuddenAttack()
{
	if (SuddenAttackCoolTime <= 0.f) //공격중은아니고 쿨기다릴때
	{
		if (Distance <= GimmickAttackRange)
		{
			int rnd = FMath::RandRange(1, 10);
			if (rnd <= 3)
			{
				SuddenAttackCoolTime = 5.f;
				AttackCoolTime = 2.f;
				isAttacking = true;
				TurnAttack(); //얘는 애니메이션자체에서 움직임
			}
			else
				SuddenAttackCoolTime = 0.9f; //한번은더할수있게
		}
		return;
	}
}

void ABossEnemy::CheckBackAttack()
{
	if (BackAttackCoolTime <= 0.f)
	{
		float dotValue = -0.4f;
		if (Distance <= GimmickAttackRange && FVector::DotProduct(GetActorForwardVector(), (TargetLocation - StartLocation).GetSafeNormal()) <= dotValue)
		{
			int rnd = FMath::RandRange(1, 10);
			if (rnd <= 2)
			{
				BackAttackCoolTime = 3.f;
				return;
			}

			isAttacking = true;
			AttackCoolTime = 2.f;
			BackAttackCoolTime = 5.f;
			BackAttack();
		}
	}
}

void ABossEnemy::Turn()
{
	if (TurnCoolTime > 0) 
		return;

	auto Dot = FVector::DotProduct((TargetLocation - StartLocation).GetSafeNormal(), GetActorRightVector());
	if (Dot >= 0.7f)//우회전
	{
		TurnCoolTime = 1.f;
		PlayAnimMontage(RTurnAnim, 1.f);
	}
	else if (Dot <= -0.7f)
	{
		TurnCoolTime = 1.f;
		PlayAnimMontage(LTurnAnim, 1.f);
	}
}

void ABossEnemy::Attack()
{
	if (isAttacking || isDie) return;

	CalculateDistFromPlayer();

	AttackCoolTime = 3.f;
	isAttacking = true;

	if (Distance <= CloseAttackRange)
	{
		int closeAttackNum = FMath::RandRange(1, 3);

		//보스의 콤보 공격들은 애니메이션 몽타주 끝부분에 nextCombo notify로 발동한다 (확률적으로)
		if (closeAttackNum == 1)
			NormalAttackCombo();
		else if (closeAttackNum == 2)
			NormalAttackCombo();
		else if (closeAttackNum == 3)
			TurnAttack();
	}
	else
	{
		int rangeAttackNum = FMath::RandRange(1, 2);

		if (rangeAttackNum == 1)
			RangeAttack1();
		else if (rangeAttackNum == 2)
			RangeAttack2();
	}
}

void ABossEnemy::BackAttack()
{
	SetActorRotation((GetActorForwardVector() * -1.f).Rotation());
	PlayAnimMontage(BackAttackAnim, 1.f);
}

void ABossEnemy::NormalAttackCombo()
{
	PlayAnimMontage(NormalAttackAnim, 1.f);
}

void ABossEnemy::RangeAttack1()
{
	PlayAnimMontage(RangeAttackAnim1, 1.f);
}

void ABossEnemy::TurnAttack()
{
	PlayAnimMontage(TurnAttackAnim, 1.f);
}

void ABossEnemy::RangeAttack2()
{
	isRotationArroundToPlayer = true;

	int attackNum = FMath::RandRange(1, 2);
	if(attackNum==1)
		PlayAnimMontage(LDodgeAnim, 1.f);
	else
		PlayAnimMontage(RDodgeAnim, 1.f);
}

void ABossEnemy::RotationArroundToPlayer()
{
	if (isRotationArroundToPlayer == false)
		return;
	
	CalculateDistFromPlayer();
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
	auto resRot = FMath::RInterpTo(GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), PlayerLookRotationSpeed);

	SetActorRotation(FRotator(GetActorRotation().Pitch, resRot.Yaw, GetActorRotation().Roll));
}
void ABossEnemy::LaunchToPlayer()
{
	GetCharacterMovement()->StopMovementImmediately();
	PlayAnimMontage(RangeAttackAnim2, 1.f);

	ProjectToTarget();
	isCheckNearGround = false;
	isParabola = true;

	FTimerHandle WaitHandle;
	float WaitTime = 0.5f; 
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			isCheckNearGround = true;
		}), WaitTime, false); 
}

void ABossEnemy::ProjectToTarget()
{
	CalculateDistFromPlayer();
	FVector outVelocity = FVector::ZeroVector;   // 결과 Velocity
	float speed = 3000.f;

	UGameplayStatics::SuggestProjectileVelocity(this, outVelocity, StartLocation, TargetLocation, speed, false,
		GetCapsuleComponent()->GetScaledCapsuleRadius(),
		0.f, ESuggestProjVelocityTraceOption::DoNotTrace);

	float alpha = (TargetLocation - StartLocation).Size() / speed;
	LaunchCharacter(outVelocity * alpha, true, true);
}

void ABossEnemy::TickParabola(float delta)
{
	if (!isParabola) return;
	float power = 50.f;
	AccumulateDownVerticalForce +=  GravitationalAcceleration * delta * power; //누적중력 += 중력가속도
	//초기세로힘에서 값이 점점 줄어들면서 포물선을 이루게 됨
	float resultOffset = (VerticalForce - AccumulateDownVerticalForce) * delta * power;//초기세로힘- 누적중력
	AddActorLocalOffset(FVector(0.F, 0.F, resultOffset)); 

	if (isCheckNearGround)
	{
		if (CheckNearGround())
		{
			isParabola = false;
			AccumulateDownVerticalForce = 0.f;
		}
	}
}

bool ABossEnemy::CheckNearGround()
{
	float range = 150.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bResult = GetWorld()->LineTraceMultiByChannel(HitResults, GetActorLocation(), GetActorLocation() - GetActorUpVector() * range, ECollisionChannel::ECC_GameTraceChannel1, Params);

	return bResult;
}

void ABossEnemy::BossDie()
{
	isDie = true;
	isAttacking = true; //동작못하게
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);

	PlayAnimMontage(DieAnim, 1.f);
	PlayBossDeathCam();
	GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), 5.f, ECameraAnimPlaySpace::CameraLocal);

	FTimerHandle WaitHandle;
	float WaitTime = 8.f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			FName path = TEXT("Blueprint'/Game/My__/PLAYER/Entrance/ToBoss2_Portal_BP.ToBoss2_Portal_BP_C'");
			UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));

			FVector location = GetActorLocation() + GetActorUpVector() * 200.f;
			FRotator rotation = GetActorRotation();
			GetWorld()->SpawnActor<AActor>(GeneratedBP, location, rotation);

		}), WaitTime, false);
}

void ABossEnemy::Boss_AttackCheck()
{
	float attack_range = 350.f;
	float attact_radius = 350.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * attack_range,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(attact_radius),
		Params);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			APlayerCharacter* player = Cast<APlayerCharacter>(HitResult.Actor);
			if (player)
			{
				player->MyTakeDamage2(MyDamage, HitedState::knock, 0.f, GetActorForwardVector() * 1500.f +GetActorUpVector()*500.f);
				return;
			}
		}
	}

}


void ABossEnemy::MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_, FVector launchVec)
{
	if (isDie)
		return;

	PlayBossHitEffects();

	HP -= damage;

	if (HPBarWidget)
		ChangeHPUi();
	
	if (HP <= 0)
		BossDie();
}