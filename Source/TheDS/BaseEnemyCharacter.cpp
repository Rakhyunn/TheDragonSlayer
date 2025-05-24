#include "BaseEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseStatComponent.h"
#include "BaseCharacter.h"

ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	stat = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));
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

void ABaseEnemyCharacter::ReceiveDamage(class ABaseCharacter* Causer, float Damage)
{
	stat->GetDamage(Damage);
	UE_LOG(LogTemp, Warning, TEXT("Remain HP: %f"), stat->GetCurrentHP());
	if (stat->GetCurrentHP() <= 0.f)
	{
		Die(Causer);
	}
}