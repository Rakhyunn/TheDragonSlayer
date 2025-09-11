#include "BTTask_BossMelee.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossBase.h"

UBTTask_BossMelee::UBTTask_BossMelee()
{
	NodeName = TEXT("Boss Melee");
}

EBTNodeResult::Type UBTTask_BossMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
    if (!Boss || !Boss->HasAuthority()) return EBTNodeResult::Failed;
    const float Now = Boss->GetWorld()->GetTimeSeconds();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (BB && NextAllowedTimeKey.SelectedKeyType)
    {
        const float NextAllowed = BB->GetValueAsFloat(NextAllowedTimeKey.SelectedKeyName);
        if (Now < NextAllowed) return EBTNodeResult::Failed;
        BB->SetValueAsFloat(NextAllowedTimeKey.SelectedKeyName, Now + Cooldown);
    }
    Boss->ServerStartAction(EBossAction::Melee, NAME_None);
    return EBTNodeResult::Succeeded;
}
