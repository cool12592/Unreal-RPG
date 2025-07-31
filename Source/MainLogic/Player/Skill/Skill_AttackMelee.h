// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_AttackMelee.generated.h"

/**
 * 
 */
UCLASS(Blueprintable) 
class NOTEBOOK_API USkill_AttackMelee : public USkill_Base
{
	GENERATED_BODY()

public:
	virtual void Init_(APlayerCharacter* player, FSkillData* skillData) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;

protected:
	virtual void ResetCount() override;

private:
	void ContinueNextCombo();
	void ApplyAttackMeleeDamage();

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pSwordHit;

	static constexpr int32 LastAttackMeleeCount = 4;
	int32 AttackMeleeCount = 0;
	bool IsAttackMeleeInputOn = false;
};
