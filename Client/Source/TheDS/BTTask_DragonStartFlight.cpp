#include "BTTask_DragonStartFlight.h"
#include "AIController.h"
#include "BossBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_DragonStartFlight::UBTTask_DragonStartFlight()
{
    NodeName = TEXT("Dragon Start Flight");
}

EBTNodeResult::Type UBTTask_DragonStartFlight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    ABossBase* Boss = AI ? Cast<ABossBase>(AI->GetPawn()) : nullptr;
    if (!Boss || !Boss->HasAuthority()) return EBTNodeResult::Failed;
    if (UCharacterMovementComponent* Move = Boss->GetCharacterMovement())
    {
        Move->SetMovementMode(MOVE_Flying);
        Move->bUseControllerDesiredRotation = true;
    }
    if (bPlayTakeOffMontage)
    {
        Boss->ServerStartAction(EBossAction::TakeOff, NAME_None);
    }
    return EBTNodeResult::Succeeded;
}
