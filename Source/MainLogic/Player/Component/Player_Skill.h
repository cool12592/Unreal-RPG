// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "SkillData.h"
#include "Skill_Base.h"
#include "Player_Skill.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnResetCountDelegate);


enum class SkillName : uint8;

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	None            UMETA(DisplayName = "None"),
	AttackMelee    UMETA(DisplayName = "Attack Melee"),
	LastAttackMelee UMETA(DisplayName = "Last Attack Melee"),
	DashAttack       UMETA(DisplayName = "Dash Attack"),
	AirLaunch      UMETA(DisplayName = "Air Launch"),
	AirCombo       UMETA(DisplayName = "Air Combo"),
	SlashWave      UMETA(DisplayName = "Slash Wave"),
	GreatSword     UMETA(DisplayName = "Great Sword"),
	ChainLightningDash      UMETA(DisplayName = "Chain Lightning Dash"),
	BlossomTempest       UMETA(DisplayName = "Blossom Tempest"),
	Ultimate       UMETA(DisplayName = "Ultimate")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NOTEBOOK_API UPlayer_Skill : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayer_Skill();

	UFUNCTION(Server, Reliable)
	void StartSkill(FName skillName);

	UFUNCTION(BlueprintCallable)
	void ResetCount();

	void MyCameraShake(float power);
	bool SweepTrace(FVector start, FVector end, float radius, TArray<FHitResult>& HitResults);

	void AddLinkedSkillCount();
	bool IsMaxLinkedSkillCount();
	void BlockadeSkill();

	FOnResetCountDelegate OnResetCount;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, USkill_Base*> SkillMapInstance;

private:
	void ResetLinkedSkillCount();

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TMap<FName, TSubclassOf<USkill_Base>> SkillMapClass;

	UPROPERTY()
	class UPlayer_State* PlayerStateComponent;  

	UPROPERTY(EditAnywhere, Category = "Skill")
	UDataTable* SkillDataTable;

	UPROPERTY()
	class APlayerCharacter* myplayer;

	int32 LinkedSkillCount = 0;
	static constexpr int32 LastLinkedSkillCount = 2;

	FTimerHandle WaitHandle;
};
