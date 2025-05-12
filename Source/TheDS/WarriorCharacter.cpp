#include "WarriorCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "CharacterAnimInstance.h"

AWarriorCharacter::AWarriorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterDefaults();		//이동속도, 점프 설정
	AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());	// AnimInstance 가져오기
	// 함수 등록
	AnimInstance->OnMontageEnded.AddDynamic(this, &AWarriorCharacter::OnAttackMontageEnded);
	AnimInstance->OnAttackHit.AddUObject(this, &AWarriorCharacter::Attack);
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
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	// 단순 애니매이션 재생
	if (AnimInstance)
	{
		bIsAttacking = false;
		AnimInstance->PlayAttackMontage();
	}
}

void AWarriorCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = true;
}