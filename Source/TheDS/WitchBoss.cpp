#include "WitchBoss.h"
#include "EnemyProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "UserPlayerController.h"
#include "TheDSPlayerState.h"
#include "InventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

AWitchBoss::AWitchBoss()
{
	MeleeRange = 0.f;
}

void AWitchBoss::DoMelee()
{
	// 마녀는 근접 공격이 없음
	return;
}

void AWitchBoss::DoRanged()
{
	if (!HasAuthority() || !PoisonProjectileClass) return;
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now < NextRangedTime) return;
	ServerStartAction(EBossAction::Ranged, NAME_None);
	NextRangedTime = Now + RangedCooldown;
}

void AWitchBoss::DoMoveOrSpecial()
{
	if (!HasAuthority()) return;
	ServerStartAction(EBossAction::Teleport, NAME_None);
}

void AWitchBoss::ServerTeleportNear(ABaseCharacter* Target)
{
	if (!Target) return;
	const FVector Offset = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(TeleportMin, TeleportMax);
	FHitResult Hit;
	SetActorLocation(Target->GetActorLocation() + Offset, true, &Hit);
}

void AWitchBoss::Die(ABaseCharacter* Causer)
{
	if (HasAuthority() && DragonPearlDataAsset) 
	{
		// 처치자의 파티원 전원 보상
		if (AUserPlayerController* KillerPC = Causer ? Cast<AUserPlayerController>(Causer->GetController()) : nullptr) 
		{
			if (ATheDSPlayerState* PS = KillerPC->GetPlayerState<ATheDSPlayerState>()) 
			{
				const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
				for (const FPartyMember& M : Members) 
				{
					if (AUserPlayerController* PC = Cast<AUserPlayerController>(M.Member->GetOwner())) 
					{
						if (ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn())) 
						{
							if (Player->InventoryComponent) 
							{ 
								Player->InventoryComponent->AddItem(DragonPearlDataAsset, 1); 
							}
							FChatMessage RewardMsg;
							RewardMsg.Sender = TEXT("시스템");
							RewardMsg.Message = TEXT("드래곤 진주를 획득하였습니다.");
							RewardMsg.Channel = EChatChannel::Global;
							PC->ServerSystemChat(RewardMsg);
						}
					}
				}
			}
		}
		bDead = true;
		OnRep_Dead();
	}
}

void AWitchBoss::OnRep_Dead()
{
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}
}

void AWitchBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWitchBoss, bDead);
}

void AWitchBoss::ServerTeleportToNearestTarget_Implementation(float SearchRange)
{
	if (ABaseCharacter* Target = FindNearestPlayer(SearchRange))
	{
		ServerTeleportNear(Target);
	}
}

void AWitchBoss::MulticastSetHidden_Implementation(bool bShow)
{
	SetActorHiddenInGame(bShow);
}
