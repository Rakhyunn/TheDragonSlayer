#include "WarriorCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "CharacterAnimInstance.h"
#include "BaseStatComponent.h"
#include "BaseEnemyCharacter.h"

AWarriorCharacter::AWarriorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	stat->SetLevel(1);
	stat->SetAttack(100.f);
	stat->SetMagic(10.f);
	stat->SetDefense(20.f);
}

void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterDefaults();		//이동속도, 점프 설정
	animInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());	// AnimInstance 가져오기
	// 함수 등록
	animInstance->OnMontageEnded.AddDynamic(this, &AWarriorCharacter::OnAttackMontageEnded);
	animInstance->OnAttackHit.AddUObject(this, &AWarriorCharacter::Attack);
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
		if (hitResult.GetActor()->ActorHasTag(TEXT("Enemy"))) {
			ABaseEnemyCharacter* hitEnemy = Cast<ABaseEnemyCharacter>(hitResult.GetActor());
			UE_LOG(LogTemp, Warning, TEXT("Attack: %f"), stat->GetAttack());
			hitEnemy->ReceiveDamage(this, stat->GetAttack());
		}
	}
	MulticastAttack();
}

void AWarriorCharacter::MulticastAttack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	// 단순 애니매이션 재생
	if (animInstance)
	{
		bIsAttacking = false;
		animInstance->PlayAttackMontage();
	}
}

void AWarriorCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = true;
}

void AWarriorCharacter::DontMove()
{
	Super::DontMove();
	animInstance->StopMove();
}
