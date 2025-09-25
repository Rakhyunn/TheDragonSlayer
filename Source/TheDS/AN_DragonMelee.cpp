#include "AN_DragonMelee.h"
#include "DragonBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"

void UAN_DragonMelee::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;
    UWorld* World = MeshComp->GetWorld();
    ADragonBoss* Dragon = Cast<ADragonBoss>(MeshComp->GetOwner());
    if (!Dragon || !World || !Dragon->HasAuthority()) return;
    const float AttackRange = 180.f;
    const float AttackRadius = 60.f;
    const float HalfHeight = AttackRange * 0.5f;
    const FVector Start = Dragon->GetActorLocation();
    const FVector End = Start + Dragon->GetActorForwardVector() * AttackRange;
    const FQuat   Rot = FRotationMatrix::MakeFromZ(Dragon->GetActorForwardVector()).ToQuat();
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DragonMelee), false, Dragon);
    FCollisionObjectQueryParams ObjParams; ObjParams.AddObjectTypesToQuery(ECC_Pawn);
    TArray<FHitResult> Hits;
    const FCollisionShape Shape = FCollisionShape::MakeCapsule(AttackRadius, HalfHeight);
    const bool bHit = World->SweepMultiByObjectType(Hits, Start, End, Rot, ObjParams, Shape, Params);
    const FVector Center = (Start + End) * 0.5f;
    DrawDebugCapsule(World, Center, HalfHeight, AttackRadius, Rot, bHit ? FColor::Green : FColor::Red, false, 1.5f, 0, 2.f);
    TSet<AActor*> Already;
    for (const FHitResult& Hit : Hits)
    {
        AActor* A = Hit.GetActor();
        if (!A || A == Dragon || Already.Contains(A) || !A->ActorHasTag("Player")) continue;
        if (ABaseCharacter* Player = Cast<ABaseCharacter>(A))
        {
            Already.Add(A);
            Player->ReceiveDamage(Dragon->Stat ? Dragon->Stat->GetAttack() : 50.f);
        }
    }
}
