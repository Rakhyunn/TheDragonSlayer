#include "BTService_FindTarget.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = TEXT("Find Target");
	Interval = 1.f;
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn)
	{
		FVector Center = Pawn->GetActorLocation();
		float Range = 500.f;
		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams QueryParams(NAME_Name, false, Pawn);
		bool Result = GetWorld()->OverlapMultiByChannel
		(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(Range),
			QueryParams
		);
		if (Result)
		{
			for (auto& OverlapResult : OverlapResults)
			{
				auto Player = Cast<ABaseCharacter>(OverlapResult.GetActor());
				if (Player)
				{
					DrawDebugSphere(GetWorld(), Center, Range, 10, FColor::Green, false, 0.5f);
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), Player);
					return;
				}
			}

		}
		DrawDebugSphere(GetWorld(), Center, Range, 10, FColor::Red, false, 0.5f);
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), nullptr);
	}
}