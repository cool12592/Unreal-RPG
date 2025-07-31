// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "SkillData.h"
#include "PlayerCharacter.h"
#include "MyAnimInstance.h"
#include "BasicEnemy.h"
#include "Skill_Base.generated.h"


UCLASS(Abstract, Blueprintable)  
class NOTEBOOK_API USkill_Base : public UObject
{
	GENERATED_BODY()

public:	
	USkill_Base();

	void Init(APlayerCharacter* player, FSkillData* skillData);
	virtual void Init_(APlayerCharacter* player, FSkillData* skillData);

	virtual void StartSkill() PURE_VIRTUAL(USkill_Base::StartSkill, );

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool CheckUseSkill();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetCoolTimePercent() const;

	void SetInfiniteCoolTime();

protected:
	UPROPERTY()
	class UPlayer_Skill* PlayerSkill;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* BasicSkillMontage;

	FSkillData* SkillData;

	UPROPERTY(BlueprintReadOnly)
	float EndCoolTime;

	UPROPERTY()
	class UPlayer_State* PlayerStateComponent;

	UPROPERTY()
	class APlayerCharacter* OwnerPlayer;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ConsumeSkillResources();

	bool CheckCoolTime();
	void RecordCoolTime();
	virtual void ResetCount();

private:
	bool bIsInfiniteCoolTime = false;
};
