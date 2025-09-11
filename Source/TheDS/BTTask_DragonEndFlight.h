#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DragonEndFlight.generated.h"

UCLASS()
class THEDS_API UBTTask_DragonEndFlight : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Flight")
    bool bPlayLandMontage = false;

public:
    UBTTask_DragonEndFlight();

public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
