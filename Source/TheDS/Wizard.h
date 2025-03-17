#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Wizard.generated.h"

UCLASS()
class THEDS_API AWizard : public ACharacter
{
	GENERATED_BODY()

public:
	AWizard();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//Declare SpringArm && Camera
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
};
