// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "MyAIController.h"
#include "BasicEnemy.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    OwnerEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (OwnerEnemy == nullptr)
        return EBTNodeResult::Failed;

    OwnerEnemy->Attack();

    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (OwnerEnemy == nullptr)
        return;

    if (OwnerEnemy->isAttacking == false || OwnerEnemy->isHited)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
