// Fill out your copyright notice in the Description page of Project Settings.


#include "WizardBossEnemy.h"
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

AWizardBossEnemy::AWizardBossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	isTurnDuringAttacking = true;
	UsedSkillNumArray.Init(0, 5);
}

// Called when the game starts or when spawned
void AWizardBossEnemy::BeginPlay()
{
	Super::BeginPlay();
	DetectRange = 10000.f;
	AttackRange = 1500.F;
	TrackToRange = 500.F;

	isBoss = true;
	AttackCoolTime = 2.f;
	MaxHP = 120.f;
	HP = MaxHP;
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	TurnSpeed = 4.f;
	MyDamage = 15.f;
}

void AWizardBossEnemy::Tick(float DeltaTime)
{
	RunCoolTime(DeltaTime);
	RotationArroundToPlayer();
	Move();
}

void AWizardBossEnemy::Attack()
{
	if (isAttacking) 
		return;

	GetCharacterMovement()->StopMovementImmediately();
	HitCount = 0;
	isAttacking = true;
	AttackCoolTime = 3.f;

	SelectSkill();
}

void AWizardBossEnemy::PlaySkill1()
{
	bRotationArroundToPlayer = true;

	int rnd = FMath::RandRange(1, 2);
	if (rnd == 1)
		PlayAnimMontage(LDodgeAnim, 1.f);
	else
		PlayAnimMontage(RDodgeAnim, 1.f);
}

void AWizardBossEnemy::PlaySkill2()
{
	PlayAnimMontage(AttackAnim2, 1.f);
}

void AWizardBossEnemy::PlaySkill3()
{
	PlayAnimMontage(AttackAnim3, 1.f);
}

void AWizardBossEnemy::PlaySkill4()
{
	isTurnDuringAttacking = false;//기모을동안
	bRotationArroundToPlayer = false;
	PlayAnimMontage(AttackAnim4, 1.f);
}

void AWizardBossEnemy::PlayUltimateSkill()
{
	UltimateCool = 30.f;
	PlayAnimMontage(AttackAnim3, 1.f);
}

void AWizardBossEnemy::ApplyDamage()
{
	float attackRange = 350.f;
	float attactRadius = 350.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * attackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(attactRadius),
		Params);


	if (!bResult)
		return;
	
	for (const FHitResult& HitResult : HitResults)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(HitResult.Actor);
		if (player)
		{
			player->MyTakeDamage2(MyDamage, HitedState::knock, 0.f, GetActorForwardVector() * 1500.f + GetActorUpVector() * 500.f);
			return;
		}
	}
}

void AWizardBossEnemy::MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_, FVector launchVec)
{
	if (HP <= 0)
		return;

	PlayBossHitEffects();

	HP -= damage;
	HitCount++;

	if (HPBarWidget)
	{
		ChangeHPUi();
	}

	isAttacking = true; //동작못하게

	if (HP <= 0)
	{
		Die();
	}
	else
	{
		PlayHitedAnim();
	}
}

void AWizardBossEnemy::RotationArroundToPlayer()
{
	if (!bRotationArroundToPlayer)
		return;
	FVector startLoc = GetActorLocation();      // 발사 지점
	FVector targetLoc = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();  // 타겟 지점.
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(startLoc, targetLoc);
	auto resRot = FMath::RInterpTo(GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 10.f);

	SetActorRotation(FRotator(GetActorRotation().Pitch, resRot.Yaw, GetActorRotation().Roll));
}

void AWizardBossEnemy::PerformNextCombo()
{
	if (CurSelectSkillNum <= 2)
		PlayAnimMontage(AttackAnim1, 1.f);
	else if (CurSelectSkillNum <= 4)
		PlaySkill2();
	else if (CurSelectSkillNum <= 6)
		PlaySkill3();
	else if (CurSelectSkillNum <= 8)
		PlaySkill4();
	else if (CurSelectSkillNum <= 10)
		PlayUltimateSkill();
}

void AWizardBossEnemy::Move()
{
	if (!isAttacking && TargetPlayer && TargetPlayer->GetDistanceTo(this) > AttackRange)
	{
		FVector lookVector = TargetPlayer->GetActorLocation() - GetActorLocation();
		lookVector.Z = 0.0f; //rotator말고 위치의z임
		LaunchCharacter(lookVector.GetSafeNormal() * 1300.f, true, true);
		auto targetRot = FRotationMatrix::MakeFromX(lookVector).Rotator();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRot, GetWorld()->GetDeltaSeconds(), 5.F));

		if (isMoving == false)
		{
			isMoving = true;
			PlayMoveAnim();
		}
	}
	else
	{
		if (isMoving == true)
		{
			isMoving = false;
			PlayMoveAnim();
		}
	}
}

void AWizardBossEnemy::SelectSkill()
{
	TArray<int32> UltimateSkills = { 9, 10 };
	const int32 MaxUsedSkillCount = 5;
	TArray<int32> SkillPool;

	// 후보 스킬 만들기
	for (int32 i = 1; i <= 10; ++i)
	{
		const bool bIsUltimate = UltimateSkills.Contains(i);
		const bool bUsedRecently = UsedSkillNumArray.Contains(i);
		const bool bUltimateOnCooldown = bIsUltimate && UltimateCool > 0.f;

		if (!bUsedRecently && !bUltimateOnCooldown)
		{
			SkillPool.Add(i);
		}
	}

	if (SkillPool.Num() > 0)
	{
		// 사용할 수 있는 후보 중에서 랜덤 선택
		const int32 RandomIndex = FMath::RandRange(0, SkillPool.Num() - 1);
		CurSelectSkillNum = SkillPool[RandomIndex];
	}
	else
	{
		// 예외 상황: 선택할 수 있는 스킬이 없다
		// 그냥 랜덤하게 하나 고름 (쿨 무시)
		CurSelectSkillNum = FMath::RandRange(1, 10);
	}

	//최근 스킬 갱신
	UsedSkillNumArray.Insert(CurSelectSkillNum, 0);
	if (UsedSkillNumArray.Num() > MaxUsedSkillCount)
	{
		UsedSkillNumArray.Pop(); // 오래된 것 제거
	}

	ActSkill();
}

void AWizardBossEnemy::ActSkill()
{
	if (CurSelectSkillNum <= 2)
		PlaySkill1();
	else
	{
		int rnd = FMath::RandRange(1, 3);
		if (rnd == 1)
		{
			bRotationArroundToPlayer = true;
			PlayAnimMontage(LDodgeAnim, 1.f);
		}
		else if (rnd == 2)
		{
			bRotationArroundToPlayer = true;
			PlayAnimMontage(RDodgeAnim, 1.f);
		}
		else
		{
			if (CurSelectSkillNum <= 4)
				PlaySkill2();
			else if (CurSelectSkillNum <= 6)
				PlaySkill3();
			else if (CurSelectSkillNum <= 8)
				PlaySkill4();
			else if (CurSelectSkillNum <= 10)
				PlayUltimateSkill();
		}
	}
}

void AWizardBossEnemy::Die()
{
	isTurnDuringAttacking = false;
	bRotationArroundToPlayer = false;
	GetCharacterMovement()->GravityScale = 1.f;
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);

	PlayAnimMontage(DieAnim, 1.f);
	PlayBossDeathCam();
	GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), 5.f, ECameraAnimPlaySpace::CameraLocal);

}

void AWizardBossEnemy::PlayHitedAnim()
{
	int rnd = FMath::RandRange(1, 2);

	if (HitCount >= 2)
	{
		PlayAnimMontage(HitedAnim, 1.0f, FName("4"));
	}
	else
	{
		if (rnd == 1)
			PlayAnimMontage(HitedAnim, 1.0f, FName("1"));
		else
			PlayAnimMontage(HitedAnim, 1.0f, FName("2"));
	}
}

void AWizardBossEnemy::RunCoolTime(float delta)
{
	AttackCoolTime -= delta;
	UltimateCool -= delta;
	ShieldCool -= delta;
}
