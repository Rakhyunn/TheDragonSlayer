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
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"

AWitchBoss::AWitchBoss()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Stat->SetLevel(8);
	Stat->SetAttack(30.f);
	Stat->SetMagic(60.f);
	MeleeRange = 0.f;
}

void AWitchBoss::Die(ABaseCharacter* Causer)
{
	if (!Causer) return;
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
		AUserPlayerController* PC = Cast<AUserPlayerController>(Causer->GetController());
		FVector Location = FVector(130.f, -570.f, 27850.f);
		PC->ServerLoadAndWarp("Village_1_", FTransform(Location), true);
		PC->ResetRaid();
	}
}

void AWitchBoss::OnRep_Dead()
{
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
		if (UBlackboardComponent* BB = AI->GetBlackboardComponent())
			BB->SetValueAsBool(TEXT("IsDead"), true);
		if (UBrainComponent* Brain = AI->GetBrainComponent())
			Brain->StopLogic(TEXT("Dead"));
	}
}

void AWitchBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWitchBoss, bDead);
}

void AWitchBoss::ResetEnemyStat()
{
	Super::ResetEnemyStat();
	if (Stat)
	{
		Stat->FullRestore();
	}
	bDead = false;
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AI->GetBlackboardComponent())
			BB->SetValueAsBool(TEXT("IsDead"), false);
		if (UBrainComponent* Brain = AI->GetBrainComponent())
			Brain->RestartLogic();
	}
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	SetActorHiddenInGame(false);
}

void AWitchBoss::MulticastSetHidden_Implementation(bool bShow)
{
	SetActorHiddenInGame(bShow);
}
