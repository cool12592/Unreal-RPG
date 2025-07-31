// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_SlashWave.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NOTEBOOK_API USkill_SlashWave : public USkill_Base
{
	GENERATED_BODY()
public:
	virtual void Init_(APlayerCharacter* player, FSkillData* skillData) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartSkill() override;

private:
	void PauseSlashWave();
	void ShotSlashWave();
	void ResumeSlashWave();
	void UpgradeWave();

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pCharge1;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pCharge2;
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pCharge3;

	UPROPERTY()
	class UParticleSystemComponent* pcCharge1;
	UPROPERTY()
	class UParticleSystemComponent* pcCharge2;
	UPROPERTY()
	class UParticleSystemComponent* pcCharge3;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> WaveActor_BP;

	UPROPERTY()
	class UMyAnimInstance* OwnerPlayerAnim;

	bool bIsCharging = false;
	bool bIsFullCharge = false;

	FTimerHandle WaitHandle;

};
