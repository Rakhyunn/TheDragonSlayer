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

	Stat->SetLevel(1);
	Stat->SetAttack(100.f);
	Stat->SetMagic(10.f);
	Stat->SetDefense(20.f);
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
	WalkSpeed = 600.f;
	RunSpeed = 900.f;
	JumpZVelocity = 500.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
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
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	float AttackRange = 100.f;
	float AttackRadius = 50.f;

	bool bHit = GetWorld()->SweepSingleByChannel(OUT HitResult,
		GetActorLocation(), GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius), Params);

	FVector Forward = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + Forward * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Forward).ToQuat();

	FColor DrawColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, Rotation, DrawColor, false, 2.f);
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior hit: %s"), *HitResult.GetActor()->GetName());
		if (HitResult.GetActor()->ActorHasTag(TEXT("Enemy"))) {
			ABaseEnemyCharacter* HitEnemy = Cast<ABaseEnemyCharacter>(HitResult.GetActor());
			UE_LOG(LogTemp, Warning, TEXT("Attack: %f"), Stat->GetAttack());
			HitEnemy->ReceiveDamage(this, Stat->GetAttack());
		}
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

void AWarriorCharacter::DontMove()
{
	Super::DontMove();
	AnimInstance->StopMove();
}
