#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindTarget.generated.h"

UCLASS()
class THEDS_API UBTService_FindTarget : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float SearchRadius = 500.f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> QueryChannel = ECC_GameTraceChannel1;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;

public:
	UBTService_FindTarget();

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
