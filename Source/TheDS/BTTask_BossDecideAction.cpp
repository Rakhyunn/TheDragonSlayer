#include "BTTask_BossDecideAction.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_BossDecideAction::UBTTask_BossDecideAction()
{
	NodeName = TEXT("Boss Decide Action");
}

EBTNodeResult::Type UBTTask_BossDecideAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB || DoNextActionKey.SelectedKeyName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Action Decided Error"));
		return EBTNodeResult::Failed;
	}
	const float Total = FMath::Max(0.001f, WalkWeight + TeleportOrFlyWeight);
	const float R = FMath::FRandRange(0.f, Total);
	const bool bTeleportOrFly = (R >= WalkWeight);
	BB->SetValueAsBool(DoNextActionKey.SelectedKeyName, bTeleportOrFly);
	UE_LOG(LogTemp, Warning, TEXT("Next Action Decided"));
	return EBTNodeResult::Succeeded;
}
