// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"
DECLARE_MULTICAST_DELEGATE(FMontageDelegate);
/**
 * 
 */
UCLASS()
class NOTEBOOK_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void AnimNotify_test(){ OnAttackHitCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_ultimate1_NextCombo() { OnUltimate1NextCombo.Broadcast(); }
	UFUNCTION()
	void AnimNotify_windRepeat(){ OnWindCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_inloop() { OnStartWindCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_AttackCheck() { OnAttackMeleeAttackCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_NextCombo() { OnAttackMeleeNextCombo.Broadcast(); }
	UFUNCTION()
	void AnimNotify_AirComboAttackCheck() { OnAirComboAttackCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_AirComboNextCombo() { OnAirComboNextCombo.Broadcast(); }
	UFUNCTION()
	void AnimNotify_MagicSwordAttackCheck() { OnMagicSwordAttackCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_UpperWindCheck() { OnUpperWindCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_RoarCheck() { OnRoarCheck.Broadcast(); }
	UFUNCTION()
	void AnimNotify_hitedPause() { OnHitedPause.Broadcast(); }
	UFUNCTION()
	void AnimNotify_SwordDanceCheck() { OnSwordDance.Broadcast(); }
	UFUNCTION()
	void AnimNotify_SwordWaveStop() { OnSwordWaveStop.Broadcast(); }
	UFUNCTION()
	void AnimNotify_SwordWave_Repeat() { OnSwordWaveRepeat.Broadcast(); }


	FMontageDelegate OnUltimate1NextCombo;

	FMontageDelegate OnAttackHitCheck;
	FMontageDelegate OnWindCheck;
	FMontageDelegate OnStartWindCheck;

	FMontageDelegate OnAttackMeleeNextCombo;
	FMontageDelegate OnAttackMeleeAttackCheck;

	FMontageDelegate OnAirComboAttackCheck;
	FMontageDelegate OnAirComboNextCombo;

	FMontageDelegate OnMagicSwordAttackCheck;
	FMontageDelegate OnUpperWindCheck;
	FMontageDelegate OnRoarCheck;

	FMontageDelegate OnHitedPause;

	FMontageDelegate OnSwordDance;

	FMontageDelegate OnSwordWaveStop;
	FMontageDelegate OnSwordWaveRepeat;
};
