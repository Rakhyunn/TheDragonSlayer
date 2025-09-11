#include "AN_WitchSpawnProjectile.h"
#include "WitchBoss.h"
#include "EnemyProjectile.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UAN_WitchSpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	AWitchBoss* Witch = Cast<AWitchBoss>(MeshComp->GetOwner());
	if (!Witch || !Witch->PoisonProjectileClass || !Witch->HasAuthority()) return;
	ABaseCharacter* Target = nullptr;
	if (AAIController* AIC = Cast<AAIController>(Witch->GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			Target = Cast<ABaseCharacter>(BB->GetValueAsObject(TEXT("TargetActor")));
		}
	}
	if (!Target)
	{
		Target = Witch->FindNearestPlayer(Witch->RangedRange);
	}
	if (!Target) return;
	const FVector Muzzle = Witch->GetActorLocation()
		+ Witch->GetActorForwardVector() * MuzzleOffset.X
		+ Witch->GetActorRightVector() * MuzzleOffset.Y
		+ Witch->GetActorUpVector() * MuzzleOffset.Z;
	const FRotator Dir = (Target->GetActorLocation() - Muzzle).Rotation();
	FActorSpawnParameters S;
	S.Owner = Witch;
	S.Instigator = Witch;
	if (AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(Witch->PoisonProjectileClass, Muzzle, Dir, S))
	{
		Projectile->Damage = Witch->Stat->GetMagic();
		Projectile->Dot = Witch->GetPoisonDot();
	}
}
