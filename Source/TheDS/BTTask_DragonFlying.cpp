#include "BTTask_DragonFlying.h"
#include "AIController.h"
#include "BossBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_DragonFlying::UBTTask_DragonFlying()
{
    NodeName = TEXT("Dragon Flying");
    bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_DragonFlying::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AI || !Boss || !Boss->HasAuthority() || !BB || !DestinationKey.SelectedKeyType) return EBTNodeResult::Failed;
    const FVector GroundDest = BB->GetValueAsVector(DestinationKey.SelectedKeyName);
    const FVector AirDest = GroundDest + FVector(0, 0, FlightHeight);
    EPathFollowingRequestResult::Type Code = AI->MoveToLocation(AirDest, AcceptanceRadius, false, false, false, true, nullptr, false);
    AI->SetFocalPoint(AirDest);
    if (Code == EPathFollowingRequestResult::Failed)        return EBTNodeResult::Failed;
    if (Code == EPathFollowingRequestResult::AlreadyAtGoal) return EBTNodeResult::Succeeded;
    if (UPathFollowingComponent* PF = AI->GetPathFollowingComponent())
    {
        MoveHandle = PF->OnRequestFinished.AddUObject(this, &UBTTask_DragonFlying::OnMoveCompleted);
        return EBTNodeResult::InProgress;
    }
    return EBTNodeResult::Succeeded;
}

void UBTTask_DragonFlying::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    if (AAIController* AI = OwnerComp.GetAIOwner())
    {
        if (UPathFollowingComponent* PF = AI->GetPathFollowingComponent())
        {
            PF->OnRequestFinished.Remove(MoveHandle);
        }
    }
}

void UBTTask_DragonFlying::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    UBehaviorTreeComponent* BT = Cast<UBehaviorTreeComponent>(GetOuter());
    if (BT)
    {
        FinishLatentTask(*BT, Result.IsSuccess() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
    }
}
