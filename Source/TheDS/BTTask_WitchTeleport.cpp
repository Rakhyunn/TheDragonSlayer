#include "BTTask_WitchTeleport.h"
#include "AIController.h"
#include "BossBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WitchTeleport::UBTTask_WitchTeleport()
{
	NodeName = TEXT("Witch Teleport");
}

EBTNodeResult::Type UBTTask_WitchTeleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Boss || !Boss->HasAuthority() || !BB) return EBTNodeResult::Failed;
	const FName KeyName = DestinationKey.SelectedKeyName;
	if (KeyName.IsNone()) return EBTNodeResult::Failed;
	FVector Dest = BB->GetValueAsVector(KeyName);
	Boss->SetActorLocation(Dest);
	return EBTNodeResult::Succeeded;
}
