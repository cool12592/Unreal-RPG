// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player_State.generated.h"

// 상태를 비트마스크로 관리 
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerStateFlags : uint8  
{
    None = 0 UMETA(Hidden),
    CanAttack = 1 << 0,  
    CanMove = 1 << 1,   
    CanRoll = 1 << 2,
    Max = 1 << 3 UMETA(Hidden), 
};
ENUM_CLASS_FLAGS(EPlayerStateFlags)

extern const uint8 CanAll;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOTEBOOK_API UPlayer_State : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayer_State();

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintPure, Category = "PlayerState")
    static uint8 GetCanAll();

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    float GetStamina() const { return CurrentStamina; }
    void HealStamina(float add);
    UFUNCTION(BlueprintCallable, Category = "Stamina")
    bool UseStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void RegenerateStamina(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "State")
    void SetAllStatesChecked(bool bChecked);

    UFUNCTION(BlueprintCallable, Category = "State")
    void SetOnlyStateChecked(uint8 OnlyState);

    UFUNCTION(BlueprintCallable, Category = "State")
    void AddState(uint8 NewState);

    UFUNCTION(BlueprintCallable, Category = "State")
    void RemoveState(uint8 StateToRemove);

    UFUNCTION(BlueprintCallable, Category = "State")
    bool HasState(uint8 CheckState) const;

    UFUNCTION(BlueprintCallable, Category = "State")
    uint8 GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "State")
    bool IsOnGround() const;

    UFUNCTION(BlueprintCallable)
    void Oninvincibility() { invincibility = true; }
    UFUNCTION(BlueprintCallable)
    void Offinvincibility() { invincibility = false; }
    bool invincibility = false;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "State", meta = (AllowPrivateAccess = "true"))
    uint8 CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
    float MaxStamina;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
    float CurrentStamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
    float StaminaRegenRate;
};
