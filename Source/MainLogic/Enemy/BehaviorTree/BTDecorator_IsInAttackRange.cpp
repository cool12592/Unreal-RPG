// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "MyAIController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BasicEnemy.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
    NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    auto ownerEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (ownerEnemy == nullptr)
        return false;

    auto playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (playerTarget == nullptr)
        return false;

    auto distance = playerTarget->GetDistanceTo(ownerEnemy);
    ownerEnemy->SetPlayerDistance(distance);
    bResult = (distance <= ownerEnemy->AttackRange);

   if (ownerEnemy->AttackCoolTime > 0) 
       return false;

    return bResult;
}
