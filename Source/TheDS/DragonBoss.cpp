#include "DragonBoss.h"
#include "EnemyProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "UserPlayerController.h"
#include "TheDSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "TheDSPlayerState.h"
#include "InventoryComponent.h"

ADragonBoss::ADragonBoss()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Stat->SetLevel(10);
	Stat->SetAttack(60.f);
	Stat->SetMagic(60.f);
}

void ADragonBoss::BeginPlay()
{
	Super::BeginPlay();
}

void ADragonBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bPhase2 && Stat && Stat->GetCurrentHP() <= Stat->GetMaxHP() * Phase2HPPercent)
	{
		ServerStartAction(EBossAction::Phase, NAME_None);
		EnterPhase2();
	}
}

void ADragonBoss::EnterPhase2()
{
	bPhase2 = true;
	if (Stat) 
	{ 
		Stat->AddAttack(10.f);
		Stat->AddMagic(10.f);
		Stat->AddDefense(5.f);
	}
}

void ADragonBoss::SetFlying(bool bFly)
{
	if (auto* Movement = GetCharacterMovement())
	{
		Movement->SetMovementMode(bFly ? MOVE_Flying : MOVE_Walking);
		if (bFly)
		{
			ServerStartAction(EBossAction::TakeOff, NAME_None);
		}
	}
}

void ADragonBoss::Die(ABaseCharacter* Causer)
{
	if (HasAuthority())
	{
		if (AUserPlayerController* KillerPC = Causer ? Cast<AUserPlayerController>(Causer->GetController()) : nullptr)
		{
			if (ATheDSPlayerState* PS = KillerPC->GetPlayerState<ATheDSPlayerState>())
			{
				const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
				for (const FPartyMember& M : Members)
				{
					if (AUserPlayerController* PC = Cast<AUserPlayerController>(M.Member->GetOwner()))
					{
						FChatMessage RewardMsg;
						RewardMsg.Sender = TEXT("시스템");
						RewardMsg.Message = TEXT("드래곤의 힘을 잠재웠습니다.");
						RewardMsg.Channel = EChatChannel::Global;
						PC->ServerSystemChat(RewardMsg);
					}
				}
			}
		}
		bDead = true;
		OnRep_Dead();
		EnableEndingInteract(true);
	}
}

void ADragonBoss::OnRep_Dead()
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

void ADragonBoss::EnableEndingInteract(bool bEnable)
{
	bCanEndingInteract = bEnable;
}

void ADragonBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADragonBoss, bDead);
	DOREPLIFETIME(ADragonBoss, bCanEndingInteract);
	DOREPLIFETIME(ADragonBoss, bEndingInProgress);
}

void ADragonBoss::Interact(ABaseCharacter* Interactor)
{
	if (!HasAuthority() || !Interactor || !bCanEndingInteract || bEndingInProgress) return;
	AUserPlayerController* UserPlayer = Cast<AUserPlayerController>(Interactor->GetController());
	if (!UserPlayer) return;
	ATheDSPlayerState* PS = UserPlayer->GetPlayerState<ATheDSPlayerState>();
	if (!PS || !PS->IsPartyLeader())
	{
		FChatMessage Msg;
		Msg.Sender = TEXT("시스템");
		Msg.Channel = EChatChannel::Global;
		Msg.Message = TEXT("파티장만 상호작용할 수 있습니다.");
		UserPlayer->ServerSystemChat(Msg);
		return;
	}
	const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
	for (const FPartyMember& M : Members)
	{
		if (auto* MPC = Cast<AUserPlayerController>(M.Member->GetOwner()))
			if (auto* C = Cast<ABaseCharacter>(MPC->GetPawn()))
				if (!C->InventoryComponent || !C->InventoryComponent->HasItem(DragonPearl, 1))
				{
					FChatMessage Need;
					Need.Sender = TEXT("시스템");
					Need.Channel = EChatChannel::Party;
					Need.Message = TEXT("모든 파티원이 드래곤 진주를 가지고 있어야 합니다.");
					UserPlayer->ServerSystemChat(Need);
					return;
				}
	}
	UserPlayer->ClientShowEndingConfirm(this, true);
}

void ADragonBoss::ResetEnemyStat()
{
	Super::ResetEnemyStat();
	if (Stat)
	{
		Stat->FullRestore();
	}
	bDead = false;
	bPhase2 = false;
	bCanEndingInteract = false;
	bEndingInProgress = false;
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
