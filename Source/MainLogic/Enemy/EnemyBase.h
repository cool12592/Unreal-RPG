// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EnemyHitedState : uint8
{
	Normal = 0   UMETA(DisplayName = "Normal"),	
	RightLeft	UMETA(DisplayName = "RightLeft"),
	Right		UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
	Knock UMETA(DisplayName = "Knock"),
};

UCLASS(Abstract, Blueprintable)
class NOTEBOOK_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();
	
	virtual void Attack() PURE_VIRTUAL(AEnemyBase::Attack, );
	UFUNCTION(BlueprintCallable)
	virtual void MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_ = 0.f, FVector launchVec = FVector(0.f, 0.f, 0.f)) PURE_VIRTUAL(AEnemyBase::MyTakeDamage, );

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeHPUi();

	UFUNCTION(BlueprintCallable)
	void SetState(bool flag);

	// 디텍트 범위 , 추적 범위, 공격범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectRange = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TrackToRange = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isTurnDuringAttacking = false; //공격중에도 회전하냐

	float AttackCoolTime = 0.0f;
	float ConfusedTime = 0.0f;
	bool isAttacking = false;
	bool isHited = false;
	bool isBoss = false;

	EnemyHitedState NowHitedState;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* HitedAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MyDamage = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 100.f;

	bool isRunHitedTime = false;
	bool isDie = false;

	float TurnSpeed = 5.0f;
	float HitedTime = 0.0f;
	float HideUITimer = 0.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
