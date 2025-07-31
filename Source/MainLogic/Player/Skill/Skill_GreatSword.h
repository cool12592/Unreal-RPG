// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_GreatSword.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API USkill_GreatSword : public USkill_Base
{
	GENERATED_BODY()
public:
	virtual void Init_(APlayerCharacter* player, FSkillData* skillData) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;

private:
	void ExecuteWideStun();
	void ApplyGreatSwordDamage();

};
