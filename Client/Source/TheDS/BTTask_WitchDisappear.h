#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WitchDisappear.generated.h"

UCLASS()
class THEDS_API UBTTask_WitchDisappear : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WitchDisappear();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
