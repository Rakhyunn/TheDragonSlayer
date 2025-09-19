#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WitchAppear.generated.h"

UCLASS()
class THEDS_API UBTTask_WitchAppear : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WitchAppear();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
