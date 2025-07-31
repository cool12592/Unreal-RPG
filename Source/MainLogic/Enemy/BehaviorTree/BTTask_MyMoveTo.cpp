// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyMoveTo.h"
#include "MyAIController.h"
#include "BasicEnemy.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_MyMoveTo::UBTTask_MyMoveTo()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MyMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    OwnerEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (OwnerEnemy == nullptr)
        return EBTNodeResult::Failed;

    UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (navSystem == nullptr)
        return EBTNodeResult::Failed;

    auto* playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (playerTarget == nullptr)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerEnemy->GetController(), OwnerComp.GetBlackboardComponent()->GetValueAsVector(AMyAIController::PatrolPosKey));
        isFinishedPatrolSetting = true;
    }
 
    return EBTNodeResult::InProgress;
}

void UBTTask_MyMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
   
    if (OwnerEnemy == nullptr)
        return;

    //3가지 상황  1. 공격중 혹은 공격받는 상태면 이동x
    //            2. 플레이어 타겟이 있을경우 A. 공격 가능상태면 이동중단. B 공격불가능상태인데 추적범위면 추적 C. 공격불가능인데 추적 범위 아니면 중단
    //            3. 플레이어 타겟 없으면 그냥 패트롤 지점으로 이동

    bool isMovable = CheckMovableCondition();
    if (isMovable == false)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        auto* playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
        if (playerTarget)
        {
            MoveToPlayerTarget(OwnerComp, playerTarget);
        }
        else  
        {
            CheckFinishedPatrol(OwnerComp);
        }
    }
}

bool UBTTask_MyMoveTo::CheckMovableCondition()
{
    if (OwnerEnemy->isAttacking || OwnerEnemy->isHited)
    {
        return false;
    }
    return true;
}

void UBTTask_MyMoveTo::MoveToPlayerTarget(UBehaviorTreeComponent& OwnerComp, APlayerCharacter* playerTarget)
{
    auto distance = playerTarget->GetDistanceTo(OwnerEnemy);
    //공격가능 상태면 중단
    if ((OwnerEnemy->AttackCoolTime <= 0.f && distance <= OwnerEnemy->AttackRange)) //공격가능
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        if (distance > OwnerEnemy->TrackToRange) //추적범위 확인 (어디까지 추적할거냐)
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerEnemy->GetController(), playerTarget->GetActorLocation()); //추적
        }
        else
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerEnemy->GetController(), OwnerEnemy->GetActorLocation());//이동중단(SimpleMoveToLocation먼이상한버그있어서일케중단함)
        }
    }
}



void UBTTask_MyMoveTo::CheckFinishedPatrol(UBehaviorTreeComponent& OwnerComp)
{
    if (isFinishedPatrolSetting && OwnerEnemy->GetVelocity().Size() < 1.f) //3상황 플레이어타깃이 없으면 그냥 패트롤위치 가서 속도0되고 중단
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
