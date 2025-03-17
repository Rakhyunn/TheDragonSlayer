#include "WizardAnimInstance.h"
#include "Wizard.h"

void UWizardAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();


}

void UWizardAnimInstance::NativeBeginPlay() {
	Super::NativeBeginPlay();

	//Set Character && Character Movement
	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn)) {
		Wizard = Cast<AWizard>(Pawn);
		if (IsValid(Wizard)) {
			CharacterMovement = Wizard->GetCharacterMovement();
			UE_LOG(LogTemp, Log, TEXT("CharacterMovement is Valid"));
		}
	}
}

void UWizardAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	//Set Speed
	if (IsValid(Wizard)) {
		Speed = Wizard->GetVelocity().Size2D();
	}
}