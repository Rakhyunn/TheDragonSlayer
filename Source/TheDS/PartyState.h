#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PartyType.h"
#include "PartyState.generated.h"

UCLASS()
class THEDS_API APartyState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// Leader 기준으로 파티 관리
	UPROPERTY()
	TMap<APlayerState*, FPartyInfo> ActiveParties;

public:
	void CreateParty(APlayerState* Leader);
	void InviteToParty(APlayerState* Leader, APlayerState* Invitee);
	void AcceptInvite(APlayerState* Invitee, APlayerState* Leader);
	void LeaveParty(APlayerState* Member);
	FPartyInfo* GetParty(APlayerState* Leader);
	bool IsPartyMember(APlayerState* Player);
	void UpdateAllPartyMemberStates(const FPartyInfo& Party);
};
