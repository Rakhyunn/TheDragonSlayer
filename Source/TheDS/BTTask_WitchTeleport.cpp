#include "BTTask_WitchTeleport.h"
#include "AIController.h"
#include "BossBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

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
	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Boss->GetWorld()))
	{
		FNavLocation NavLoc;
		if (NavSys->ProjectPointToNavigation(Dest, NavLoc, FVector(200.f, 200.f, 200.f)))
		{
			Dest = NavLoc.Location;
		}
	}
	Boss->SetActorLocation(Dest);
	return EBTNodeResult::Succeeded;
}
