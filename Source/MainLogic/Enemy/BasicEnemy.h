// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.h" 
#include "BasicEnemy.generated.h"


UCLASS()
class NOTEBOOK_API ABasicEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicEnemy();
		
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	virtual void Attack() override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void Attack_Multicast();

	UFUNCTION(BlueprintCallable)
	void ApplyDamage();
	UFUNCTION(BlueprintCallable)
	bool CheckPlayerShiled();

	virtual void MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_ = 0.f, FVector launchVec = FVector(0.f, 0.f, 0.f)) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnSoul();

	UFUNCTION(BlueprintImplementableEvent)
	void HideWeapon();

	UFUNCTION(BlueprintCallable)
	float GetPlayerDistance();

	UFUNCTION()
	void SetPlayerDistance(float dist);

	UFUNCTION(BlueprintCallable)
	void StartHolding(float delay);

	UFUNCTION(BlueprintCallable)
	void EndHolding();

	UFUNCTION()
	void StartResetGravityTimer(float time = 1.5f);
	UFUNCTION()
	void ClearGravityTimer();
	UFUNCTION()
	void ResetGravity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isParryEnable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	AActor* AttackedMeTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	class UParticleSystem* BlockParticle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* AttackAnim;
	
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* BlockAnim;

	UFUNCTION(BlueprintCallable)
	void ResetCount();
	
	UFUNCTION(BlueprintCallable)
	void StartHitedPause();

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<class AActor> HoldingRing_BP;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<class AActor> Soul_BP;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int isRight = 0;

	UPROPERTY()
	AActor* HoldingRingActor;
	bool isArcher = false;

private:
	void EndHitedPause();
	void ClearHitedTime();
	void TickHitTimer(float delta);
	void TickHideUITimer(float delta);
	void TickAttackCoolTime(float delta);
	void CalculateForward();
	void CalculateHitedState(EnemyHitedState hit);
	
	FVector OldForward;
	FTimerHandle GravityTM;
	float PlayerDistance = 0.f;;
};
