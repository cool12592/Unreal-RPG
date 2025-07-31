// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_DashAttack.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NOTEBOOK_API USkill_DashAttack : public USkill_Base
{
	GENERATED_BODY()
public:
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> DashAttach_BP;


};
