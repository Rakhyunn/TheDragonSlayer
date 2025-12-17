#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalType.h"
#include "InteractInterface.h"
#include "PortalActor.generated.h"

UCLASS()
class THEDS_API APortalActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	EPortalType PortalType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	FName TargetMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	FTransform TargetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	FName RequiredMap;

public:	
	APortalActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Interact(class ABaseCharacter* Interactor) override;

	FORCEINLINE EPortalType GetPortalType() const { return PortalType; }
	FORCEINLINE FName GetTargetMap() const { return TargetMap; }
	FORCEINLINE FTransform GetTargetTransform() const { return TargetTransform; }
	FORCEINLINE FName GetRequiredMap() const { return RequiredMap; }
};
