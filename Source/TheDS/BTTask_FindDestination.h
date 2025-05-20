#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindDestination.generated.h"

UCLASS()
class THEDS_API UBTTask_FindDestination : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindDestination();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
