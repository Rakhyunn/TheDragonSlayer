#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossMelee.generated.h"

UCLASS()
class THEDS_API UBTTask_BossMelee : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float Cooldown = 1.5f;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector NextAllowedTimeKey;

public:
	UBTTask_BossMelee();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
