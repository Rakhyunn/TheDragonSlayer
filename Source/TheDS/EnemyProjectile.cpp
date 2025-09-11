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
	Collision->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 1200.f;
	Movement->MaxSpeed = 1200.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->ProjectileGravityScale = 0.f;
}

void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) 
	{ 
		Destroy(); 
		return; 
	}
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(Other)) 
	{
		Player->ReceiveDamage(Damage);
		if (Dot.DPS > 0.f)
		{
			if (auto* OwnerBoss = Cast<ABossBase>(GetOwner()))
			{
				OwnerBoss->ApplyDot(Player, Dot);
			}
		}
	}
	Destroy();
}

