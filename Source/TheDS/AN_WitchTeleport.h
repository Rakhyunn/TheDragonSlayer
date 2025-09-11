#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_WitchTeleport.generated.h"

UENUM(BlueprintType)
enum class EWitchTeleportStage : uint8 
{ 
    Disappear,
    Teleport,
    Appear
};

UCLASS()
class THEDS_API UAN_WitchTeleport : public UAnimNotify
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch|Teleport")
    EWitchTeleportStage Stage = EWitchTeleportStage::Teleport;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch|Teleport")
    bool bToggleCollision = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch|Teleport")
    bool bToggleHidden = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch|Teleport")
    float TeleportSearchRange = 2500.f;

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
