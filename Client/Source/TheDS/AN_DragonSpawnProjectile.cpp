#include "AN_DragonSpawnProjectile.h"
#include "DragonBoss.h"
#include "EnemyProjectile.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UAN_DragonSpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	ADragonBoss* Dragon = Cast<ADragonBoss>(MeshComp->GetOwner());
	if (!Dragon || !Dragon->FireProjectileClass || !Dragon->HasAuthority()) return;
	ABaseCharacter* Target = nullptr;
	if (AAIController* AIC = Cast<AAIController>(Dragon->GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			Target = Cast<ABaseCharacter>(BB->GetValueAsObject(TEXT("TargetActor")));
		}
	}
	if (!Target)
	{
		Target = Dragon->FindNearestPlayer(Dragon->RangedRange);
	}
	if (!Target) return;
	const FVector Muzzle = Dragon->GetActorLocation()
		+ Dragon->GetActorForwardVector() * MuzzleOffset.X
		+ Dragon->GetActorRightVector() * MuzzleOffset.Y
		+ Dragon->GetActorUpVector() * MuzzleOffset.Z;
	const FRotator Dir = (Target->GetActorLocation() - Muzzle).Rotation();
	FActorSpawnParameters S;
	S.Owner = Dragon;
	S.Instigator = Dragon;
	if (AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(Dragon->FireProjectileClass, Muzzle, Dir, S))
	{
		Projectile->Damage = Dragon->Stat->GetMagic();
		Projectile->Dot = Dragon->GetBurnDot();
	}
}
