#include "BTTask_WitchAppear.h"
#include "AIController.h"
#include "WitchBoss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"

UBTTask_WitchAppear::UBTTask_WitchAppear()
{
	NodeName = TEXT("Witch Appear");
}

EBTNodeResult::Type UBTTask_WitchAppear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	AWitchBoss* Witch = AI ? Cast<AWitchBoss>(AI->GetPawn()) : nullptr;
	if (!Witch || !Witch->HasAuthority()) return EBTNodeResult::Failed;
	Witch->MulticastSetHidden(false);
	if (UCapsuleComponent* Cap = Witch->FindComponentByClass<UCapsuleComponent>())
	{
		Cap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	return EBTNodeResult::Succeeded;
}
