#include "TheDSPlayerState.h"
#include "GameFramework/Pawn.h"
#include "BaseStatComponent.h"
#include "BaseCharacter.h"
#include "UserPlayerController.h"
#include "Net/UnrealNetwork.h"

int32 ATheDSPlayerState::GetLevel() const
{
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(GetPawn()))
	{
		if (Player->stat)
		{
			return Player->stat->GetLevel();
		}
	}
	return 0;
}

void ATheDSPlayerState::SetPartyLeader(APlayerState* Leader)
{
	PartyLeader = Leader;
}

APlayerState* ATheDSPlayerState::GetPartyLeader() const
{
	return PartyLeader;
}

void ATheDSPlayerState::OnRep_PartyLeader()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (AUserPlayerController* UserPC = Cast<AUserPlayerController>(PC))
		{
			UserPC->ClientUpdatePartyUI();
		}
	}
}

void ATheDSPlayerState::OnRep_PartyMembers()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (AUserPlayerController* UserPC = Cast<AUserPlayerController>(PC))
		{
			UserPC->ClientUpdatePartyUI();
		}
	}
}

void ATheDSPlayerState::SetReplicatedPartyMembers(const TArray<FPartyMember>& Members)
{
	PartyMembers = Members;
	OnRep_PartyMembers();
}

void ATheDSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATheDSPlayerState, PartyLeader);
	DOREPLIFETIME(ATheDSPlayerState, PartyMembers);
}