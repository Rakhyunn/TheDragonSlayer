#include "WarriorCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

AWarriorCharacter::AWarriorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterDefaults();
}

void AWarriorCharacter::SetCharacterDefaults()
{
	walkSpeed = 600.f;
	runSpeed = 900.f;
	jumpZVelocity = 500.f;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
}

void AWarriorCharacter::Attack()
{
	Super::Attack();

	if (!bIsAttacking) return;
	ServerAttack();
}

void AWarriorCharacter::ServerAttack_Implementation()
{
	if (!HasAuthority()) return;
	FHitResult hitResult;
	FCollisionQueryParams params(NAME_None, false, this);

	float attackRange = 100.f;
	float attackRadius = 50.f;

	bool bHit = GetWorld()->SweepSingleByChannel(OUT hitResult,
		GetActorLocation(), GetActorLocation() + GetActorForwardVector() * attackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(attackRadius), params);

	FVector forward = GetActorForwardVector() * attackRange;
	FVector center = GetActorLocation() + forward * 0.5f;
	float halfHeight = attackRange * 0.5f + attackRadius;
	FQuat rotation = FRotationMatrix::MakeFromZ(forward).ToQuat();

	FColor DrawColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), center, halfHeight, attackRadius, rotation, DrawColor, false, 2.f);
	if (bHit && hitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior hit: %s"), *hitResult.GetActor()->GetName());
	}
	MulticastAttack();
}

void AWarriorCharacter::MulticastAttack_Implementation()
{
	// 단순 애니매이션 재생
	if (AttackMontage)
	{
		bIsAttacking = false;
		float length = AttackMontage->GetPlayLength();
		PlayAnimMontage(AttackMontage);
		GetWorldTimerManager().SetTimer(AttackResetTimerHandle, this, &AWarriorCharacter::ResetAttack, length, false);
	}
}