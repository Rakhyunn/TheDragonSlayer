#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WitchTeleport.generated.h"

UCLASS()
class THEDS_API UBTTask_WitchTeleport : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WitchTeleport();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
