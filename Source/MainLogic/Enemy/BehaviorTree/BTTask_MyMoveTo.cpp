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

    //3���� ��Ȳ  1. ������ Ȥ�� ���ݹ޴� ���¸� �̵�x
    //            2. �÷��̾� Ÿ���� ������� A. ���� ���ɻ��¸� �̵��ߴ�. B ���ݺҰ��ɻ����ε� ���������� ���� C. ���ݺҰ����ε� ���� ���� �ƴϸ� �ߴ�
    //            3. �÷��̾� Ÿ�� ������ �׳� ��Ʈ�� �������� �̵�

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
    //���ݰ��� ���¸� �ߴ�
    if ((OwnerEnemy->AttackCoolTime <= 0.f && distance <= OwnerEnemy->AttackRange)) //���ݰ���
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        if (distance > OwnerEnemy->TrackToRange) //�������� Ȯ�� (������ �����Ұų�)
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerEnemy->GetController(), playerTarget->GetActorLocation()); //����
        }
        else
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerEnemy->GetController(), OwnerEnemy->GetActorLocation());//�̵��ߴ�(SimpleMoveToLocation���̻��ѹ����־�����ߴ���)
        }
    }
}



void UBTTask_MyMoveTo::CheckFinishedPatrol(UBehaviorTreeComponent& OwnerComp)
{
    if (isFinishedPatrolSetting && OwnerEnemy->GetVelocity().Size() < 1.f) //3��Ȳ �÷��̾�Ÿ���� ������ �׳� ��Ʈ����ġ ���� �ӵ�0�ǰ� �ߴ�
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
