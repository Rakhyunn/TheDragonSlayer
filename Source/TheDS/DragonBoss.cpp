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

ADragonBoss::ADragonBoss()
{
}

void ADragonBoss::BeginPlay()
{
	Super::BeginPlay();
	SetFlying(true);
	ServerStartAction(EBossAction::TakeOff, NAME_None);
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

void ADragonBoss::DoMelee()
{
	if (!HasAuthority()) return;
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now < NextMeleeTime) return;
	ServerStartAction(EBossAction::Melee, NAME_None);
	NextMeleeTime = Now + MeleeCooldown;
}

void ADragonBoss::DoRanged()
{
	if (!HasAuthority() || !FireProjectileClass) return;
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now < NextRangedTime) return;
	ServerStartAction(EBossAction::Ranged, NAME_None);
	NextRangedTime = Now + RangedCooldown;
}

void ADragonBoss::DoMoveOrSpecial()
{
	// 날기 이동(애니메이션 및 실제 드래곤 보스가 이동)
}

void ADragonBoss::EnterPhase2()
{
	bPhase2 = true;
	if (Stat) 
	{ 
		Stat->AddAttack(20.f);
		Stat->AddMagic(20.f);
		Stat->AddDefense(15.f);
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
		// Todo: 드래곤과의 인터랙션 활성화(파티장만 가능)
	}
}

void ADragonBoss::OnRep_Dead()
{
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}
}

void ADragonBoss::EnableEndingInteract(bool bEnable)
{
	bCanEndingInteract = bEnable;
	if (bEnable)
	{ 
		Tags.AddUnique(TEXT("BossEnding")); 
	}
	else
	{ 
		Tags.Remove(TEXT("BossEnding")); 
	}
}

void ADragonBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADragonBoss, bDead);
	DOREPLIFETIME(ADragonBoss, bCanEndingInteract);
}

void ADragonBoss::Interact(ABaseCharacter* Interactor)
{
	if (!Interactor || !bCanEndingInteract) return;

	AUserPlayerController* UserPlayer = Cast<AUserPlayerController>(Interactor->GetController());
	if (UserPlayer)
	{
		// Todo: 상호작용 시 파티원 모두의 소비 인벤토리에서 드래곤의 진주가 있으면 파티장에게 진주 사용 여부 묻고 사용 시 엔딩으로 넘어감
	}
}
