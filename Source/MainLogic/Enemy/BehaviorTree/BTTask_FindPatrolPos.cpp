// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"

#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
    NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ownerPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (ownerPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(ownerPawn->GetWorld());
    if (navSystem == nullptr)
        return EBTNodeResult::Failed;

    FVector origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AMyAIController::HomePosKey);
    FNavLocation nextPatrol;

    if (navSystem->GetRandomPointInNavigableRadius(origin, 1500.0f, nextPatrol))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(AMyAIController::PatrolPosKey, nextPatrol.Location);
        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}