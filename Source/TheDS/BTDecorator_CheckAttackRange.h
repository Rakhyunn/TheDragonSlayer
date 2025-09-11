#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckAttackRange.generated.h"

UCLASS()
class THEDS_API UBTDecorator_CheckAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere)
	float Range = 200.f;

public:
	UBTDecorator_CheckAttackRange();

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};
