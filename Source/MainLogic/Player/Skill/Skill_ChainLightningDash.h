// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_ChainLightningDash.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NOTEBOOK_API USkill_ChainLightningDash : public USkill_Base
{
	GENERATED_BODY()
public:
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;
};
