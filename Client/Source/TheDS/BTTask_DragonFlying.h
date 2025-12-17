#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DragonFlying.generated.h"

UCLASS()
class THEDS_API UBTTask_DragonFlying : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "Flight")
    struct FBlackboardKeySelector DestinationKey;   // Vector

    UPROPERTY(EditAnywhere, Category = "Flight")
    float FlightHeight = 800.f;                     // 공중 높이

    UPROPERTY(EditAnywhere, Category = "Flight")
    float AcceptanceRadius = 180.f;                 // 도착 허용 반경

public:
    UBTTask_DragonFlying();

public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
    FDelegateHandle MoveHandle;
    void OnMoveCompleted(FAIRequestID RequestID, const struct FPathFollowingResult& Result);
};
