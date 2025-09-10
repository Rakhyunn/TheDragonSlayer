#include "BossAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ABossAIController::ABossAIController()
{
}

void ABossAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (!HasAuthority()) return;     // 서버에서만 실행

    if (BehaviorTree)
    {
        UBlackboardData* BBAsset = BehaviorTree->BlackboardAsset;
        UBlackboardComponent* BBComp = nullptr;
        if (BBAsset && UseBlackboard(BBAsset, BBComp))
        {
            RunBehaviorTree(BehaviorTree);
        }
    }
}

void ABossAIController::OnUnPossess()
{
    Super::OnUnPossess();
}
