#include "BaseEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ABaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterDefaults();
}

void ABaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseEnemyCharacter, walkSpeed);
	DOREPLIFETIME(ABaseEnemyCharacter, runSpeed);
}

void ABaseEnemyCharacter::SetCharacterDefaults()
{
	walkSpeed = 500.f;
	runSpeed = 800.f;
	jumpZVelocity = 420.f;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
}

void ABaseEnemyCharacter::ResetAttack()
{
	bIsAttacking = true;
}