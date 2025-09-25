#include "AN_WitchTeleport.h"
#include "WitchBoss.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UAN_WitchTeleport::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;
    AWitchBoss* Witch = Cast<AWitchBoss>(MeshComp->GetOwner());
    if (!Witch) return;
	switch (Stage)
	{
    case EWitchTeleportStage::Disappear:        
        if (Witch->HasAuthority())
        {
            UE_LOG(LogTemp, Warning, TEXT("Witch Disappear Notify"));
            if (bToggleCollision && Witch->GetCapsuleComponent())
            {
                Witch->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            if (AAIController* AI = Cast<AAIController>(Witch->GetController()))
            {
                AI->StopMovement();
            }
        }
        if (bToggleHidden)
        {
            Witch->MulticastSetHidden(true);
        }
        break;
    case EWitchTeleportStage::Teleport:
        if (Witch->HasAuthority())
        {
            UE_LOG(LogTemp, Warning, TEXT("Witch Teleport Notify"));
            Witch->ServerTeleportToNearestTarget(TeleportSearchRange);
        }
        break;
    case EWitchTeleportStage::Appear:        
        if (bToggleHidden)
        {
            UE_LOG(LogTemp, Warning, TEXT("Witch Appear Notify"));
            Witch->MulticastSetHidden(false);
        }
        if (Witch->HasAuthority())
        {
            if (bToggleCollision) Witch->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
        break;
	}
}
