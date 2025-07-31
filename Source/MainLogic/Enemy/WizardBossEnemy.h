// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"

#include "WizardBossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API AWizardBossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:

	AWizardBossEnemy();

	virtual void Attack() override;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage();

	virtual void MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_ = 0.f, FVector launchVec = FVector(0.f, 0.f, 0.f)) override;

	UFUNCTION(BlueprintCallable)
	void RotationArroundToPlayer();

	void PerformNextCombo();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayMoveAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBossDeathCam();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBossHitEffects();

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* AttackAnim1;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* AttackAnim2;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* AttackAnim3;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* AttackAnim4;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* UltimateAttackAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* LDodgeAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* RDodgeAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* DieAnim;


	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* LTurnAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* RTurnAnim;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* MyCurrentMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotationArroundToPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShieldCool = 1.f;

	UPROPERTY()
	AActor* TargetPlayer;

private:
	void PlaySkill1();
	void PlaySkill2();
	void PlaySkill3();
	void PlaySkill4();
	void PlayUltimateSkill();

	void RunCoolTime(float delta);
	void Move();
	void SelectSkill();
	void ActSkill();
	void Die();
	void PlayHitedAnim();

	TArray<int32> UsedSkillNumArray;
	int32 CurSelectSkillNum = 0;
	float UltimateCool = 5.f;
	int HitCount = 0;
	bool isParabola = false;
	bool bCheckNearGround = false;
};
