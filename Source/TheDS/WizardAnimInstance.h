#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WizardAnimInstance.generated.h"

UCLASS()
class THEDS_API UWizardAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	//Set Variables for Animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation")
	float Horizontal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Animation")
	float Vertical;

public:
	UPROPERTY(VisibleAnywhere)
	class AWizard* Wizard;
	UPROPERTY(VisibleAnywhere)
	class UCharacterMovementComponent* CharacterMovement;
};
