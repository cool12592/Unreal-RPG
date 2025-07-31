// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Skill.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "MyMatineeCameraShake.h"
#include "Engine.h"
#include "MyAnimInstance.h"
#include "BasicEnemy.h"
#include "PlayerStatus.h"
#include "Skill_Base.h"


// Sets default values for this component's properties
UPlayer_Skill::UPlayer_Skill()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}



// Called when the game starts
void UPlayer_Skill::BeginPlay()
{
	Super::BeginPlay();
	myplayer = Cast<APlayerCharacter>(GetOwner());
	checkf(myplayer, TEXT("Ptr is nullptr"));

	PlayerStateComponent = myplayer->FindComponentByClass<UPlayer_State>();
	checkf(PlayerStateComponent, TEXT("Ptr is nullptr"));



	if (!SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillDataTable is nullptr! Cannot load skill data."));
		return;
	}

	TArray<FName> RowNames = SkillDataTable->GetRowNames();
	static const FString ContextString(TEXT("Fetching Row Data"));

	for (const FName& SkillName : RowNames)
	{
		FSkillData* Row = SkillDataTable->FindRow<FSkillData>(SkillName, ContextString);
		if (Row == nullptr)
		{
			continue;
		}


		FSkillData* FoundSkillData = SkillDataTable->FindRow<FSkillData>(SkillName, TEXT("Skill Lookup"));
		if (!FoundSkillData)
		{
			UE_LOG(LogTemp, Error, TEXT("Skill lookup failed! SkillName: %s"), *SkillName.ToString());
			continue;
		}

		if (SkillMapClass.Contains(SkillName) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Skill not set! SkillName: %s"), *SkillName.ToString());
			continue;
		}
		TSubclassOf<USkill_Base> FoundSkillClass = *SkillMapClass.Find(SkillName);


		USkill_Base* NewSkill = NewObject<USkill_Base>(this, *FoundSkillClass);
		if (NewSkill)
		{
			SkillMapInstance.Add(SkillName, NewSkill);
			NewSkill->Init(myplayer, FoundSkillData);
			UE_LOG(LogTemp, Warning, TEXT("Skill %s Added"), *SkillName.ToString());
		}
	}
}



void UPlayer_Skill::StartSkill_Implementation(FName skillName)
{
	if(SkillMapInstance.Contains(skillName) == false)
		return;

	USkill_Base* skill = *SkillMapInstance.Find(skillName);
	if (skill)
	{
		skill->StartSkill();
	}

}


void UPlayer_Skill::ResetCount()
{
	OnResetCount.Broadcast();
	myplayer->GetCharacterMovement()->GravityScale = 1.f;
	myplayer->Offinvincibility();

	PlayerStateComponent->SetAllStatesChecked(true);


	GetWorld()->GetTimerManager().ClearTimer(WaitHandle);
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		ResetLinkedSkillCount();
	}), 1.f, false);
}

void UPlayer_Skill::AddLinkedSkillCount()
{
	LinkedSkillCount++;
}

bool UPlayer_Skill::IsMaxLinkedSkillCount()
{
	if (LinkedSkillCount == LastLinkedSkillCount)
		return true;

	return false;
}

void UPlayer_Skill::BlockadeSkill()
{
	for (const TPair<FName, USkill_Base*>& Pair : SkillMapInstance)
	{
		FName SkillName = Pair.Key;
		USkill_Base* Skill = Pair.Value;

		if (Skill)
		{
			Skill->SetInfiniteCoolTime();
		}
	}
}

void UPlayer_Skill::ResetLinkedSkillCount()
{
	LinkedSkillCount = 0;
}

void UPlayer_Skill::MyCameraShake(float power)
{
	if (myplayer->GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), power, ECameraAnimPlaySpace::CameraLocal);
}

bool UPlayer_Skill::SweepTrace(FVector start, FVector end, float radius, TArray<FHitResult>& HitResults)
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(myplayer);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		start,
		end,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(radius),
		Params);

	return bResult;
}


