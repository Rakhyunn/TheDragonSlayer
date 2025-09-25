#include "BTTask_BossRanged.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossBase.h"

UBTTask_BossRanged::UBTTask_BossRanged()
{
	NodeName = TEXT("Boss Ranged");
}

EBTNodeResult::Type UBTTask_BossRanged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
    if (!Boss || !Boss->HasAuthority()) return EBTNodeResult::Failed;
    const float Now = Boss->GetWorld()->GetTimeSeconds();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (BB && !NextAllowedTimeKey.SelectedKeyName.IsNone())
    {
        const float NextAllowed = BB->GetValueAsFloat(NextAllowedTimeKey.SelectedKeyName);
        if (Now < NextAllowed) return EBTNodeResult::Failed;
        BB->SetValueAsFloat(NextAllowedTimeKey.SelectedKeyName, Now + Cooldown);
    }
    Boss->ServerStartAction(EBossAction::Ranged, FName("RangedAttack"));
    return EBTNodeResult::Succeeded;
}
