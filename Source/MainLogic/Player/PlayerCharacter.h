// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitedState.h"
#include "PlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class SkillName : uint8
{
	DashAttack = 0   UMETA(DisplayName = "DashAttack"),	
	AirLaunch	UMETA(DisplayName = "AirLaunch"),
	AirCombo UMETA(DisplayName = "AirCombo"),
	SlashWave UMETA(DisplayName = "SlashWave"),
	GreatSword UMETA(DisplayName = "GreatSword"),
	ChainLightningDash UMETA(DisplayName = "ChainLightningDash"),
	BlossomTempest UMETA(DisplayName = "BlossomTempest"),
	Ultimate UMETA(DisplayName = "Ultimate"),
};

UCLASS()
class NOTEBOOK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CheckCanUseItem();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable)
	void ResetCount();

	UFUNCTION(BlueprintCallable)
	void Oninvincibility();
	UFUNCTION(BlueprintCallable)
	void Offinvincibility();

	UFUNCTION(BlueprintCallable)
	void MyTakeDamage2(float damage, HitedState hit, float hitedTime_ = 1.f, FVector launchVec = FVector(0.f, 0.f, 0.f));

	UFUNCTION(BlueprintImplementableEvent, Category = "Default")
	void AddToGold(int gold);

	UFUNCTION(Server, Reliable)
	void HP_HEAL(float add);
	UFUNCTION(Server, Reliable)
	void STAMINA_HEAL(float add);

	//Anim Montage
	UPROPERTY()
	class UMyAnimInstance* MyAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isDefence = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isDashComboCharge = false;

public:

	/// <summary>
	/// Blueprint Implement Functions
	/// </summary>
	/// 
	UFUNCTION(BlueprintImplementableEvent)
	void PlayDamageCameraEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlaySwordMeshRotatorToOrigin();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayTimeDilationEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlaySwordSound();
	UFUNCTION(BlueprintImplementableEvent)
	void CreateWall();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayChainLightningDash();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCameraZoomIn();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayGreatSwordCameraEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ResumeSequence();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void KnockBack(float damage, float hitedTime_, FVector launchVec);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ReSpawn();

	UFUNCTION(BlueprintCallable)
	void BlockadeSkill();
	
	UFUNCTION()
	void StartResetGravityTimer(float time = 1.5f);
	UFUNCTION()
	void ClearGravityTimer();
	UFUNCTION()
	void ResetGravity();

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* RollAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* DefenceAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* JumpAnim;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	class UPlayer_Skill* Player_Skill;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	class UPlayer_Health* Player_Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class AMyPlayerController* MyController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayer_State* Player_State;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float ForwardDirValue = 0.f;
	UPROPERTY(Replicated, BlueprintReadOnly)
	float RightDirValue = 0.f;

	static constexpr float BasicStaminaCost = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCursurOn = false;


private:

	UFUNCTION(Server, Reliable)
	void SetForwardDirValue(float value);
	UFUNCTION(Server, Reliable)
	void SetRightDirValue(float value);
	UFUNCTION(Server, Reliable)
	void Roll();
	UFUNCTION(NetMulticast, Reliable)
	void Roll_Multicast();

	void LookUp(float AxisValue);
	void Turn(float AxisValue);
	void MoveForWard(float value);
	void MoveRight(float value);
	
	void StartJump();
	void StopJump();
	
	void StartDefence();
	void EndDefence();
	int DirToIndex(float Forward, float Right);
	FTimerHandle LadingTH;

};
