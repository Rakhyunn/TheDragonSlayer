#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossDecideAction.generated.h"

UCLASS()
class THEDS_API UBTTask_BossDecideAction : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float WalkWeight = 1.0f;

	UPROPERTY(EditAnywhere)
	float TeleportOrFlyWeight = 1.0f;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector DoNextActionKey;

public:
	UBTTask_BossDecideAction();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
