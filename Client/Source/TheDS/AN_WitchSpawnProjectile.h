#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_WitchSpawnProjectile.generated.h"

UCLASS()
class THEDS_API UAN_WitchSpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch|Ranged")
	FVector MuzzleOffset = FVector(120.f, 0.f, 80.f);

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
