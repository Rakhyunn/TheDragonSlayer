#include "WarriorCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AWarriorCharacter::AWarriorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) // 서버일 때만
	{
		SetCharacterDefaults();
	}
}

void AWarriorCharacter::SetCharacterDefaults()
{
	walkSpeed = 600.f;
	runSpeed = 900.f;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->JumpZVelocity = 500.f;
}

void AWarriorCharacter::Attack()
{
	Super::Attack();
	ServerAttack();
}

void AWarriorCharacter::ServerAttack_Implementation()
{
	if (!HasAuthority()) return;
	// 충돌 여부 판단 로직 적용
	MulticastAttack();
}

void AWarriorCharacter::MulticastAttack_Implementation()
{
	// 단순 애니매이션 재생
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}