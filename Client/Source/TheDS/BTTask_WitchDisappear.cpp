#include "BTTask_WitchDisappear.h"
#include "AIController.h"
#include "WitchBoss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"

UBTTask_WitchDisappear::UBTTask_WitchDisappear()
{
	NodeName = TEXT("Witch Disappear");
}

EBTNodeResult::Type UBTTask_WitchDisappear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	AWitchBoss* Witch = AI ? Cast<AWitchBoss>(AI->GetPawn()) : nullptr;
	if (!Witch || !Witch->HasAuthority()) return EBTNodeResult::Failed;
	AI->StopMovement(); // 이동 중지
	Witch->ServerStartAction(EBossAction::Teleport, NAME_None);
	if (UCapsuleComponent* Cap = Witch->FindComponentByClass<UCapsuleComponent>())
	{
		Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Witch->MulticastSetHidden(true);
	return EBTNodeResult::Succeeded;
}
