// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Base.h"
#include "Skill_AirCombo.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API USkill_AirCombo : public USkill_Base
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
	void ApplyAirComboDamage();
	void ApplySlashStormDamage();

	void HoldCharacter(ACharacter* character);
	void ReadySlashStorm();

	void StartSlashStorm();
	void HoldTargets();
	void SlowTargets();
	void CheckFinishAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pTeleportCharge;
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pTeleportEnter;
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* pTeleportExit;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SlashStormMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UpgradeSkillMontage;

	UPROPERTY()
	UAnimMontage* CurrentSkillMontage;

	UPROPERTY()
	class ABasicEnemy* LastHitEnemy;

	TArray<FVector> AirComboPosArray;
	TArray<FRotator> AirComboRotationArray;

	FTimerHandle WaitHandle;
	FTimerHandle WaitHandle2;
	FTimerHandle WaitHandle_slow;

	static constexpr int32 LastAirComboCount2 = 6;
	static constexpr float LaunchPower_AirCombo = 1500.f;
	int32 AirComboCount = 0;
	bool bAirComboMeleeInputOn = false;
	bool bIsMaxLinkedSkillCount = false;

};
