// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitedState.h"
#include "Player_Health.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOTEBOOK_API UPlayer_Health : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayer_Health();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void MyTakeDamage2(float damage, HitedState hitType, float hitedTime_ = 1.f, FVector launchVec = FVector(0.f, 0.f, 0.f));
	
	UFUNCTION(NetMulticast, Reliable)
	void HP_HEAL(float add);
	UFUNCTION(NetMulticast, Reliable)
	void STAMINA_HEAL(float add);

	UFUNCTION(NetMulticast, Reliable)
	void ReSpawn();

	void StartHitedPause();
	void ClearHitedTime();

	HitedState NowHitedState = HitedState::normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool isRunHitedTime = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MyHp = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHp = 100.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* HPPotionAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* StaminaPotionAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* ReSpawnAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* Hited_Anim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* DeathAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* BlockAnim;

private:
	void TickHited(float delta);
	void EndHitedPause();

	UPROPERTY()
	class APlayerCharacter* Myplayer;
	UPROPERTY()
	class UPlayer_State* Player_State;

	FTimerHandle HitedTimer;

	float NowHitedTime = 0.f;
	float HitedTime = 0.f;
};
