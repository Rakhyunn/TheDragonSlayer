#include "BTTask_DragonEndFlight.h"
#include "AIController.h"
#include "BossBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_DragonEndFlight::UBTTask_DragonEndFlight()
{
	NodeName = TEXT("Dragon End Flight");
}

EBTNodeResult::Type UBTTask_DragonEndFlight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
    if (!Boss || !Boss->HasAuthority()) return EBTNodeResult::Failed;
    if (UCharacterMovementComponent* Move = Boss->GetCharacterMovement())
    {
        Move->SetMovementMode(MOVE_Walking);
        Move->bUseControllerDesiredRotation = false;
    }
    if (bPlayLandMontage)
    {
        Boss->ServerStartAction(EBossAction::Land, NAME_None);
    }
    return EBTNodeResult::Succeeded;
}
