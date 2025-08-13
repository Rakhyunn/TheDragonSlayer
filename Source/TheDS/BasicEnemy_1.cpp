#include "BasicEnemy_1.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "CharacterAnimInstance.h"
#include "BaseStatComponent.h"
#include "BaseCharacter.h"
#include "EnemyDropData.h"
#include "DropItemActor.h"
#include "DropMoneyActor.h"

ABasicEnemy_1::ABasicEnemy_1()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Stat->SetLevel(1);
	Stat->SetAttack(30.f);
	Stat->SetEnemyExp(10);
}

void ABasicEnemy_1::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterDefaults();		//이동속도, 점프 설정
	AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());	// AnimInstance 가져오기
	// 함수 등록
	AnimInstance->OnMontageEnded.AddDynamic(this, &ABasicEnemy_1::OnAttackMontageEnded);
	AnimInstance->OnAttackHit.AddUObject(this, &ABasicEnemy_1::Attack);
}

void ABasicEnemy_1::SetCharacterDefaults()
{
	WalkSpeed = 600.f;
	RunSpeed = 900.f;
	JumpZVelocity = 500.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
}

void ABasicEnemy_1::Attack()
{
	Super::Attack();

	if (!bIsAttacking) return;
	ServerAttack();
}

void ABasicEnemy_1::ServerAttack_Implementation()
{
	if (!HasAuthority()) return;
	FHitResult HitResult;
	FCollisionQueryParams params(NAME_None, false, this);

	float AttackRange = 100.f;
	float AttackRadius = 50.f;

	bool bHit = GetWorld()->SweepSingleByChannel(OUT HitResult,
		GetActorLocation(), GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius), params);

	FVector Forward = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + Forward * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Forward).ToQuat();

	FColor DrawColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, Rotation, DrawColor, false, 2.f);
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior hit: %s"), *HitResult.GetActor()->GetName());
		if (HitResult.GetActor()->ActorHasTag(TEXT("Player"))) {
			ABaseCharacter* HitPlayer = Cast<ABaseCharacter>(HitResult.GetActor());
			HitPlayer->ReceiveDamage(Stat->GetAttack());
		}
	}
	MulticastAttack();
}

void ABasicEnemy_1::MulticastAttack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	// 단순 애니매이션 재생
	if (AnimInstance)
	{
		bIsAttacking = false;
		AnimInstance->PlayAttackMontage();
	}
}

void ABasicEnemy_1::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = true;
}

void ABasicEnemy_1::Die(ABaseCharacter* Causer)
{
	Super::Die(Causer);
	DropLoot();
}

void ABasicEnemy_1::DropLoot()
{
	Super::DropLoot();
	if (!DropTable || DropRowName.IsNone()) return;

	const FString Context = TEXT("DropLootLookup");
	FEnemyDropData* DropData = DropTable->FindRow<FEnemyDropData>(DropRowName, Context);
	if (!DropData) return;

	// 아이템 드랍
	for (const FDropItemInfo& ItemInfo : DropData->DropItems)
	{
		if (FMath::FRand() <= ItemInfo.DropRate && ItemInfo.Item)
		{
			// 아이템 스폰
			FVector DropLocation = GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(30.f, 100.f);
			ADropItemActor* DropItem = GetWorld()->SpawnActor<ADropItemActor>(DropItemClass, DropLocation, FRotator::ZeroRotator);
			if (DropItem)
			{
				DropItem->Init(ItemInfo.Item, ItemInfo.Quantity);
			}
		}
	}

	// 돈 드랍
	if (FMath::FRand() <= DropData->MoneyDropRate)
	{
		int32 money = FMath::RandRange(DropData->MinMoney, DropData->MaxMoney);
		// 돈 스폰
		FVector DropLocation = GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(30.f, 100.f);
		ADropMoneyActor* DropMoney = GetWorld()->SpawnActor<ADropMoneyActor>(DropMoneyClass, DropLocation, FRotator::ZeroRotator);
		if (DropMoney)
		{
			DropMoney->Init(money);
		}
	}
}
