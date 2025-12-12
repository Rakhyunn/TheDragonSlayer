#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossAIController.generated.h"

UCLASS()
class THEDS_API ABossAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABossAIController();

protected:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BehaviorTree;

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
