#include "WarriorAnimInstance.h"
#include "Warrior.h"

void UWarriorAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();


}

void UWarriorAnimInstance::NativeBeginPlay() {
	Super::NativeBeginPlay();

	//Set Character && Character Movement
	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn)) {
		Warrior = Cast<AWarrior>(Pawn);
		if (IsValid(Warrior)) {
			CharacterMovement = Warrior->GetCharacterMovement();
			UE_LOG(LogTemp, Log, TEXT("CharacterMovement is Valid"));
		}
	}
}

void UWarriorAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	//Set Speed
	if (IsValid(Warrior)) {
		Speed = Warrior->GetVelocity().Size2D();
	}
}