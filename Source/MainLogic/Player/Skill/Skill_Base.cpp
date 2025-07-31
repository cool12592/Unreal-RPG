// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_Base.h"
#include "Player_Skill.h"
#include "PlayerCharacter.h"
// Sets default values for this component's properties
USkill_Base::USkill_Base()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void USkill_Base::Init(APlayerCharacter* player, FSkillData* skillData)
{
	OwnerPlayer = player;
	checkf(OwnerPlayer, TEXT("Not Player Set"));

	PlayerSkill = OwnerPlayer->FindComponentByClass<UPlayer_Skill>();
	checkf(PlayerSkill, TEXT("Not Player_Skill Set"));
	PlayerStateComponent = OwnerPlayer->FindComponentByClass<UPlayer_State>();
	checkf(PlayerStateComponent, TEXT("Not Player_State Set"));

	SkillData = skillData;
	checkf(SkillData, TEXT("Not SkillData Set"));

	Init_(OwnerPlayer, SkillData);


}

void USkill_Base::Init_(APlayerCharacter* player, FSkillData* skillData)
{
}


// Called when the game starts
//void USkill_Base::BeginPlay()
//{
//	Super::BeginPlay();
//
//	// ...
//	
//}
//
//
//// Called every frame
//void USkill_Base::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}



bool USkill_Base::CheckUseSkill()
{
	if (!PlayerStateComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent: PlayerStateComponent가 없습니다!"));
		return false;
	}
	
	if (!SkillData)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SkillComponent: %s 스킬을 찾을 수 없습니다!"), *UEnum::GetValueAsString(SkillType));
		return false;
	}

	if (CheckCoolTime() == false)
		return false;

	if (PlayerStateComponent->GetStamina() < SkillData->StaminaCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent: 스테미너가 부족합니다!"));
		return false;
	}

	if (SkillData->IsOnGround && !PlayerStateComponent->IsOnGround())
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent: 공중에서는 사용할 수 없습니다!"));
		return false;
	}

	if (!PlayerStateComponent->HasState(SkillData->RequiredState))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent: 현재 상태에서는 스킬을 사용할 수 없습니다!"));
		return false;
	}

	return true;
}

void USkill_Base::ConsumeSkillResources()
{
	if (SkillData == nullptr || PlayerStateComponent == nullptr)
		return;

	if (PlayerStateComponent->UseStamina(SkillData->StaminaCost) == false)
		return;

	PlayerStateComponent->RemoveState(SkillData->ClearedState);
	RecordCoolTime();

	return;
}

bool USkill_Base::CheckCoolTime()
{
	if (bIsInfiniteCoolTime)
		return false;

	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	if (EndCoolTime <= CurrentTime)
	{
		return true;
	}

	return false;
}

void USkill_Base::RecordCoolTime()
{
	EndCoolTime = GetWorld()->GetRealTimeSeconds() + SkillData->Cooldown;
}

void USkill_Base::ResetCount()
{
}

float USkill_Base::GetCoolTimePercent() const
{
	if (bIsInfiniteCoolTime)
		return 1.0f;

	if (SkillData == nullptr)
		return 0.0f;

	float RemainingTime = EndCoolTime - GetWorld()->GetRealTimeSeconds();

	// 남은 시간 비율 (0.0 ~ 1.0 범위)
	float CoolTimePercent = FMath::Clamp(RemainingTime / SkillData->Cooldown, 0.0f, 1.0f);

	return CoolTimePercent;
}

void USkill_Base::SetInfiniteCoolTime()
{
	bIsInfiniteCoolTime = true;
}


