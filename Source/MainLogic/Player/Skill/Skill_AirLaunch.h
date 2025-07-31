// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_AirLaunch.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NOTEBOOK_API USkill_AirLaunch : public USkill_Base
{
	GENERATED_BODY()

public:
	virtual void Init_(APlayerCharacter* player, FSkillData* skillData) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;

protected:
	virtual void ResetCount() override;

private:
	void ApplyAirLaunchDamage();

	int32 AirLaunchCount = 0;
	static constexpr float LaunchPower = 1500.f;

};
