#include "EnemyProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseCharacter.h"

AEnemyProjectile::AEnemyProjectile()
{
 	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(16.f);
	SetRootComponent(Collision);
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlapBegin);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 1200.f;
	Movement->MaxSpeed = 1200.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->ProjectileGravityScale = 0.f;
}

void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	Collision->IgnoreActorWhenMoving(GetOwner(), true);
}

void AEnemyProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		Destroy();
		return;
	}
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		if (ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor))
		{
			Player->ReceiveDamage(Damage);
			if (Dot.DPS > 0.f)
				if (auto* OwnerBoss = Cast<ABossBase>(GetOwner()))
					OwnerBoss->ApplyDot(Player, Dot);
		}
	}
	Destroy();
}

