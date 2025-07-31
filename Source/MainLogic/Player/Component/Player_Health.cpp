// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Health.h"
#include "PlayerCharacter.h"
#include "Player_State.h"
#include "Misc/OutputDeviceNull.h"
#include "MyAnimInstance.h"
// Sets default values for this component's properties
UPlayer_Health::UPlayer_Health()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayer_Health::BeginPlay()
{
	Super::BeginPlay();

	Myplayer = Cast<APlayerCharacter>(GetOwner());
	checkf(Myplayer, TEXT("Ptr is nullptr"));

	Player_State = Myplayer->FindComponentByClass<UPlayer_State>();
	checkf(Player_State, TEXT("Ptr is nullptr"));
	
}


// Called every frame
void UPlayer_Health::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickHited(DeltaTime);
}

void UPlayer_Health::TickHited(float delta)
{
	if (isRunHitedTime)
	{
		NowHitedTime += delta;
		if (NowHitedTime >= HitedTime)
		{
			EndHitedPause();
			ClearHitedTime();
		}
	}
}

void UPlayer_Health::StartHitedPause()
{
	Myplayer->MyAnim->Montage_Pause(Myplayer->GetCurrentMontage());
	NowHitedTime = 0.f;
	isRunHitedTime = true;
	Player_State->AddState(static_cast<uint8>(EPlayerStateFlags::CanRoll));
}
void UPlayer_Health::EndHitedPause()
{
	NowHitedState = HitedState::normal;
	Myplayer->MyAnim->Montage_Resume(Myplayer->GetCurrentMontage());
}
void UPlayer_Health::ClearHitedTime()
{
	NowHitedTime = 0.f;
	isRunHitedTime = false;
}

void UPlayer_Health::HP_HEAL_Implementation(float add)
{
	Player_State->SetAllStatesChecked(false);
	Myplayer->PlayAnimMontage(HPPotionAnim, 1.0f);
	MyHp += add;
	if (MyHp > MaxHp) MyHp = MaxHp;
}
void UPlayer_Health::STAMINA_HEAL_Implementation(float add)
{
	Player_State->SetAllStatesChecked(false);
	Myplayer->PlayAnimMontage(StaminaPotionAnim, 1.0f);

	Player_State->HealStamina(add);
}

void UPlayer_Health::ReSpawn_Implementation()
{
	Myplayer->PlayAnimMontage(ReSpawnAnim, 1.0f);
	Myplayer->SetActorLocation(Myplayer->GetActorLocation() + Myplayer->GetActorUpVector() * 1000.f);

	MyHp = MaxHp;
}

void UPlayer_Health::MyTakeDamage2(float damage, HitedState hitType, float hitedTime_, FVector launchVec)
{
	if (Player_State->invincibility) 
		return;

	if (MyHp <= 0)
		return;

	Player_State->SetAllStatesChecked(false);
	HitedTime = hitedTime_; //TICK에서 작동
	MyHp -= damage;

	FOutputDeviceNull pAR;
	Myplayer->PlayDamageCameraEffect();

	if (Myplayer->isDefence) //방어중에 캔슬된상황
	{
		Myplayer->isDefence = false;
		Myplayer->PlaySwordMeshRotatorToOrigin();
	}

	if (MyHp <= 0)
	{
		Player_State->invincibility = true;
		Myplayer->PlayAnimMontage(DeathAnim, 1.0f);
		return;
	}

	if (hitType == HitedState::RightLeft)
	{
		if (NowHitedState == HitedState::right)
		{
			NowHitedState = HitedState::left;
			Myplayer->PlayAnimMontage(Hited_Anim, 1.0f, FName("2"));
		}
		else
		{
			NowHitedState = HitedState::right;
			Myplayer->PlayAnimMontage(Hited_Anim, 1.0f, FName("1"));
		}
	}
	else if (hitType == HitedState::knock)
	{
		if (NowHitedState == HitedState::knock)//이미 누워있다면
			Myplayer->PlayAnimMontage(Hited_Anim, 1.0f, FName("4"));
		else
			Myplayer->PlayAnimMontage(Hited_Anim, 1.0f, FName("3"));

		NowHitedState = HitedState::knock;
	}
	else if (hitType == HitedState::Block)
	{
		Myplayer->PlayAnimMontage(BlockAnim, 1.0f);
		NowHitedState = HitedState::Block;
	}

	if (launchVec != FVector(0.f, 0.f, 0.f))
		Myplayer->LaunchCharacter(launchVec, true, true);
}




