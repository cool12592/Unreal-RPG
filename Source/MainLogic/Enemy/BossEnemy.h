// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABossEnemy();
	virtual void Attack() override;
	virtual void MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_ = 0.f, FVector launchVec = FVector(0.f, 0.f, 0.f)) override;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Boss_AttackCheck();

	UFUNCTION(BlueprintCallable)
	void LaunchToPlayer();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBossDeathCam();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBossHitEffects();

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* NormalAttackAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* RangeAttackAnim1;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* RangeAttackAnim2;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* TurnAttackAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* BackAttackAnim;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isRotationArroundToPlayer = false;

private:
	void RunCoolTime(float DeltaTime);
	void CalculateDistFromPlayer();
	void CheckSuddenAttack();
	void CheckBackAttack();
	void Turn();
	bool CheckNearGround();
	void BossDie();
	void RotationArroundToPlayer();
	void ProjectToTarget();
	void TickParabola(float delta);
	void NormalAttackCombo();
	void RangeAttack1();
	void TurnAttack();
	void RangeAttack2();
	void BackAttack();

	bool bFirstAttack = true;

	static constexpr float GimmickAttackRange = 600.f;
	static constexpr float CloseAttackRange = 900.f;
	static constexpr float OriginAttackCoolTime = 2.f;
	float TurnCoolTime;
	float BackAttackCoolTime;
	float SuddenAttackCoolTime;

	bool isParabola=false;
	bool isCheckNearGround=false;
	
	static constexpr float GravitationalAcceleration=8.f;
	static constexpr float VerticalForce=140.f;
	float AccumulateDownVerticalForce;

	FVector StartLocation;
	FVector TargetLocation;
	float Distance;

	float PlayerLookRotationSpeed = 10.f;
};
