#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindTarget.generated.h"

UCLASS()
class THEDS_API UBTService_FindTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindTarget();

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
