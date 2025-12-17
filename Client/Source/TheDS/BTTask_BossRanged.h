#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossRanged.generated.h"

UCLASS()
class THEDS_API UBTTask_BossRanged : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float Cooldown = 2.5f;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector NextAllowedTimeKey;

public:
	UBTTask_BossRanged();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
