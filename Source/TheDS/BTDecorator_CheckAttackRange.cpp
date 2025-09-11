#include "BTDecorator_CheckAttackRange.h"
#include "BaseCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckAttackRange::UBTDecorator_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
}

bool UBTDecorator_CheckAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Result = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr)
		return false;

	auto Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
		return false;

	if (Target->GetDistanceTo(Pawn) > Range)
		return false;

	return Result;
}