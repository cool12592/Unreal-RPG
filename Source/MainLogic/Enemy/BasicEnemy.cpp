// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemy.h"
#include "MyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "PlayerCharacter.h"
#include "Soul.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"

// Sets default values
ABasicEnemy::ABasicEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MaxHP = 100.F;
	HP = MaxHP;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.f, 0.0f);

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	HPBarWidget->SetupAttachment(GetRootComponent());
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> HPBarFinder(TEXT("Blueprint'/Game/My__/Enemy/AI/UI_HPBar.UI_HPBar_C'"));
	if (HPBarFinder.Succeeded())
	{
		HPBarWidget->SetWidgetClass(HPBarFinder.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
		HPBarWidget->SetVisibility(false);
	}
	
	static ConstructorHelpers::FObjectFinder<UClass> HoldRingFinder(TEXT("Blueprint'/Game/My__/SkillBP/Special/Tie/TieActor_BP.TieActor_BP_C'"));
	if (HoldRingFinder.Object)
	{
		HoldingRing_BP = HoldRingFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UClass> SoulFinder(TEXT("Blueprint'/Game/My__/Soul/Soul_BP.Soul_BP_C'"));
	if (SoulFinder.Object)
	{
		Soul_BP = SoulFinder.Object;
	}
}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasicEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickHitTimer(DeltaTime);
	TickHideUITimer(DeltaTime);
	TickAttackCoolTime(DeltaTime);
	CalculateForward();
}

void ABasicEnemy::CalculateForward()
{
	if (OldForward != GetActorForwardVector())
	{
		auto res = FVector::DotProduct(GetActorRightVector(), GetActorForwardVector() - OldForward);
		if (res > 0)//우회전
		{
			isRight = 1;
		}
		else if (res < 0)
		{
			isRight = -1;
		}
	}
	else
		isRight = 0;

	OldForward = GetActorForwardVector();
}

void ABasicEnemy::Attack_Implementation()
{
	Attack_Multicast();
}

void ABasicEnemy::Attack_Multicast_Implementation()
{
	if (isAttacking) return;
	if (isHited) return;
	isAttacking = true;
	PlayAnimMontage(AttackAnim, 1.f);
}

void ABasicEnemy::ApplyDamage()
{
	float attackRange = 100.0f;
	float attactRadius = 200.0f;
	float knockBackPower = 1600.0f;
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
			player->MyTakeDamage2(MyDamage, HitedState::RightLeft, 0.f, GetActorForwardVector() * knockBackPower);
			return;
		}
	}
}
bool ABasicEnemy::CheckPlayerShiled()
{
	float attackRange = 100.0f;
	float attactRadius = 200.0f;
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
		return false;

	for (const FHitResult& HitResult : HitResults)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(HitResult.Actor);
		if (player && player->isDefence) //디펜스상태이고
		{
			if (FVector::DotProduct(player->GetActorLocation() - GetActorLocation(), GetActorForwardVector()) > 0) //그가 내 앞에있고
			{
				if (FVector::DotProduct(player->GetActorForwardVector(), GetActorForwardVector()) <= -0.3f) //이정도각도라면
				{
					UGameplayStatics::SpawnEmitterAttached(BlockParticle, GetMesh(), FName("Pelvis"));//Pelvis에붙임

					MyTakeDamage(nullptr, 0.f, EnemyHitedState::Normal, 2.f);
					PlayAnimMontage(BlockAnim, 1.0f);
					return true;

				}
			}
		}
	}
	
	return false;
}

void ABasicEnemy::MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_ , FVector launchVec)
{
	if (HP <= 0 && GetCharacterMovement()->IsMovingOnGround())
		return;

	AttackCoolTime = 1.5;
	isRunHitedTime = false; //pause에서부터 돌아감
	isHited = true;
	HitedTime = hitedTime_; //TICK에서 작동

	HP -= damage;

	if (HPBarWidget)
	{
		HPBarWidget->SetVisibility(true);
		HideUITimer = 5.f; 
		ChangeHPUi();
	}

	if (HP <= 0) //0인데 공중인경우는 다 knock으로받음
	{
		if (HoldingRingActor)
			HoldingRingActor->Destroy();
		hit = EnemyHitedState::Knock;
	}

	if (attacker != nullptr)
	{
		auto controller = Cast<AMyAIController>(GetController());
		controller->SetTarget(attacker);
		AttackedMeTarget = attacker;
	}

	CalculateHitedState(hit);

	if (launchVec != FVector(0.f, 0.f, 0.f))
		LaunchCharacter(launchVec, true, true);
}

void ABasicEnemy::CalculateHitedState(EnemyHitedState hit)
{
	if (hit == EnemyHitedState::RightLeft)
	{
		if (NowHitedState == EnemyHitedState::Right)
		{
			NowHitedState = EnemyHitedState::Left;
			PlayAnimMontage(HitedAnim, 1.0f, FName("2"));
		}
		else
		{
			NowHitedState = EnemyHitedState::Right;
			PlayAnimMontage(HitedAnim, 1.0f, FName("1"));
		}
	}
	else if (hit == EnemyHitedState::Knock)
	{
		if (NowHitedState == EnemyHitedState::Knock)//이미 누워있다면
			PlayAnimMontage(HitedAnim, 1.0f, FName("4"));
		else
			PlayAnimMontage(HitedAnim, 1.0f, FName("3"));

		NowHitedState = EnemyHitedState::Knock;
	}
}

void ABasicEnemy::ResetCount()
{
}

void ABasicEnemy::StartHitedPause()
{
	isRunHitedTime = true;
	GetMesh()->GetAnimInstance()->Montage_Pause(GetCurrentMontage());
}

void ABasicEnemy::EndHitedPause()
{
	NowHitedState = EnemyHitedState::Normal;
	GetMesh()->GetAnimInstance()->Montage_Resume(GetCurrentMontage());
}

void ABasicEnemy::ClearHitedTime()
{
	HitedTime = 0.f;
	isRunHitedTime = false;
}

void ABasicEnemy::TickHitTimer(float delta)
{
	if (isRunHitedTime && GetCharacterMovement()->IsMovingOnGround()) ////땅에서만 타이머돔
	{
		HitedTime -= delta;

		if (HP <= 0)
		{
			if (isDie == false)
			{
				isDie = true;

				if (HoldingRingActor)
				{
					HoldingRingActor->Destroy();
				}

				SpawnSoul();

				if (GetWorld()->IsServer()) //직접 타이머 재고 destroy하는건 서버만
				{
					FTimerHandle WaitHandle;
					float WaitTime = 7.f; //시간을 설정하고
					GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
						{
							HideWeapon();
							this->Destroy();
						}), WaitTime, false); 
				}
			}

			return; //밑으로 못가게
		}
		
		if (HitedTime <= 0.f)
		{
			EndHitedPause();
			ClearHitedTime();
			//	SetAllState(true);
		}
	}
}

void ABasicEnemy::TickHideUITimer(float delta)
{
	if (HideUITimer <= 0)
		return;

	HideUITimer -= delta;
	if (HideUITimer <= 0)
	{
		HPBarWidget->SetVisibility(false);
	}
}

void ABasicEnemy::TickAttackCoolTime(float delta)
{
	if (0 < AttackCoolTime)
		AttackCoolTime -= delta;
}

void ABasicEnemy::StartHolding(float delay)
{
	FTimerHandle WaitHandle;
	float WaitTime = delay; //시간을 설정하고
					
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		if (HP <= 0.f)
			return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		FRotator rotator(0.f, 0.f, 0.f);
		FVector  SpawnLocation = GetActorLocation();
		HoldingRingActor = GetWorld()->SpawnActor<AActor>(HoldingRing_BP, FTransform(GetActorRotation(), SpawnLocation, FVector(3.f)), SpawnParams);
	}), WaitTime, false); 	
}

void ABasicEnemy::EndHolding()
{
	if (HoldingRingActor)
		HoldingRingActor->Destroy();

	MyTakeDamage(AttackedMeTarget,100.f, EnemyHitedState::Knock, 8.f, FVector(0.F, 0.F, 500.F));
}

float ABasicEnemy::GetPlayerDistance()
{
	return PlayerDistance;
}

void ABasicEnemy::SetPlayerDistance(float dist)
{
	PlayerDistance = dist;
}

void ABasicEnemy::StartResetGravityTimer(float time)
{
	GetWorldTimerManager().ClearTimer(GravityTM);
	GetWorldTimerManager().SetTimer(GravityTM, this, &ABasicEnemy::ResetGravity, time, false);
}

void ABasicEnemy::ResetGravity()
{
	GetCharacterMovement()->GravityScale = 1.f;
	CustomTimeDilation = 1.f;
}

void ABasicEnemy::ClearGravityTimer()
{
	GetWorldTimerManager().ClearTimer(GravityTM);
}
