#include "BTTask_WitchTeleport.h"
#include "AIController.h"
#include "BossBase.h"

UBTTask_WitchTeleport::UBTTask_WitchTeleport()
{
	NodeName = TEXT("Witch Teleport");
}

EBTNodeResult::Type UBTTask_WitchTeleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
	if (!Boss || !Boss->HasAuthority()) return EBTNodeResult::Failed;
	Boss->ServerStartAction(EBossAction::Teleport, NAME_None);
	return EBTNodeResult::Succeeded;
}
