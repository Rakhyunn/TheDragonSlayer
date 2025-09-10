#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DragonStartFlight.generated.h"

UCLASS()
class THEDS_API UBTTask_DragonStartFlight : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Flight")
	bool bPlayTakeOffMontage = false;

public:
    UBTTask_DragonStartFlight();

public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
