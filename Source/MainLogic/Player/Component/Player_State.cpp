// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_State.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

const uint8 CanAll = static_cast<uint8>(EPlayerStateFlags::CanAttack) |
static_cast<uint8>(EPlayerStateFlags::CanMove) |
static_cast<uint8>(EPlayerStateFlags::CanRoll);

UPlayer_State::UPlayer_State()
{
    PrimaryComponentTick.bCanEverTick = true; 

    MaxStamina = 100.0f;
    CurrentStamina = MaxStamina;
    StaminaRegenRate = 5.0f; // 초당 5 스테미너 회복
}

void UPlayer_State::BeginPlay()
{
    Super::BeginPlay();

    CurrentState = CanAll;

}

void UPlayer_State::AddState(uint8 NewState)
{
    CurrentState |= NewState; // 비트 OR 연산으로 추가
}

void UPlayer_State::RemoveState(uint8 StateToRemove)
{
    CurrentState &= ~StateToRemove; // 비트 AND NOT 연산으로 제거
}

bool UPlayer_State::HasState(uint8 CheckState) const
{
    if (CheckState == 0) return true;

    return (CurrentState & CheckState) != 0;
}

bool UPlayer_State::IsOnGround() const
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter != nullptr)
        return OwnerCharacter->GetCharacterMovement()->IsMovingOnGround();

    return false;
}

uint8 UPlayer_State::GetCanAll()
{
    return CanAll;
}

void UPlayer_State::HealStamina(float add)
{
    CurrentStamina += add; 
    if(MaxStamina < CurrentStamina) 
        CurrentStamina = MaxStamina;
}

bool UPlayer_State::UseStamina(float Amount)
{
    if (CurrentStamina >= Amount)
    {
        CurrentStamina -= Amount;
        if (CurrentStamina < 0.0f)
        {
            CurrentStamina = 0.0f;
        }
        return true;
    }
    return false;
}

void UPlayer_State::SetAllStatesChecked(bool bChecked)
{
    if (bChecked)
    {
        CurrentState = CanAll;
    }
    else
    {
        CurrentState = static_cast<uint8>(EPlayerStateFlags::None);
    }
}

void UPlayer_State::SetOnlyStateChecked(uint8 OnlyState)
{
    CurrentState = static_cast<uint8>(EPlayerStateFlags::None) | OnlyState;
}


void UPlayer_State::RegenerateStamina(float DeltaTime)
{
    if (CurrentState != CanAll) //hited time도 아닐때 해야되나
        return;

    CurrentStamina += StaminaRegenRate * DeltaTime;
    if (CurrentStamina > MaxStamina)
    {
        CurrentStamina = MaxStamina;
    }
}

void UPlayer_State::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    RegenerateStamina(DeltaTime);
}


