// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_ChainLightningDash.h"
								
void USkill_ChainLightningDash::StartSkill_Implementation()
{
	if (CheckUseSkill() == false)
		return;

	ConsumeSkillResources();

	OwnerPlayer->isDashComboCharge = true;

	OwnerPlayer->PlayAnimMontage(BasicSkillMontage, 1.0f);

	OwnerPlayer->Oninvincibility();
	OwnerPlayer->PlayChainLightningDash();

}