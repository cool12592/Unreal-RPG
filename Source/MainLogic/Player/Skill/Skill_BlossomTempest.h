// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_BlossomTempest.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API USkill_BlossomTempest : public USkill_Base
{
	GENERATED_BODY()
public:
	virtual void Init_(APlayerCharacter* player, FSkillData* skillData) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;

private:
	void ContinueNextCombo();
	void ApplyBlossomDamage();
	void HoldCharacter(ACharacter* character);
	void SlowTargets(TArray<FHitResult>& OutHitResults);

	const FVector LaunchPower = FVector(0.f, 0.f, 2000.f);

	int32 ComboCount = 0;
	int32 ComboRepeat = 0;
	bool bIsFirst = true;
};
