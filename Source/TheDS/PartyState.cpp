#include "PartyState.h"
#include "TheDSPlayerState.h"
#include "UserPlayerController.h"

void APartyState::CreateParty(APlayerState* Leader)
{
	if (!Leader || ActiveParties.Contains(Leader)) return;

	FPartyInfo NewParty;
	NewParty.Leader = Leader;

	FPartyMember SelfMember;
	SelfMember.Member = Leader;
	if (ATheDSPlayerState* PS = Cast<ATheDSPlayerState>(Leader))
	{
		SelfMember.Nickname = PS->GetNickname();
	}
	else
	{
		SelfMember.Nickname = Leader->GetPlayerName();
	}

	if (ATheDSPlayerState* PlayerState = Cast<ATheDSPlayerState>(Leader))
	{
		SelfMember.Level = PlayerState->GetLevel();
	}

	NewParty.Members.Add(SelfMember);
	ActiveParties.Add(Leader, NewParty);

	if (ATheDSPlayerState* PlayerState = Cast<ATheDSPlayerState>(Leader))
	{
		PlayerState->SetPartyLeader(Leader);
	}

	UE_LOG(LogTemp, Log, TEXT("CreateParty: %s has created a new party"), *Leader->GetPlayerName());

	UpdateAllPartyMemberStates(NewParty);
}

void APartyState::InviteToParty(APlayerState* Leader, APlayerState* Invitee)
{
	if (!Leader || !Invitee) return;
	if (!ActiveParties.Contains(Leader)) return;

	// 이미 파티에 있는 사람은 초대 불가
	if (ATheDSPlayerState* InviteePS = Cast<ATheDSPlayerState>(Invitee))
	{
		if (InviteePS->GetPartyLeader()) return;
	}

	// 여기서는 그냥 로그로 확인
	UE_LOG(LogTemp, Log, TEXT("%s has invited %s to the party"),
		*Leader->GetPlayerName(), *Invitee->GetPlayerName());

	// 실제 초대 UI는 Client RPC 등으로 따로 구현해야 함
	if (APlayerController* PC = Cast<APlayerController>(Invitee->GetOwner()))
	{
		AUserPlayerController* UserPC = Cast<AUserPlayerController>(PC);
		if (UserPC)
		{
			UserPC->ClientShowPartyInvite(Leader);
		}
	}
}

void APartyState::AcceptInvite(APlayerState* Invitee, APlayerState* Leader)
{
	if (!Leader || !Invitee || !ActiveParties.Contains(Leader)) return;

	FPartyInfo& Party = ActiveParties[Leader];
	// 파티 인원은 4명까지로 제한
	if (Party.Members.Num() >= 4) return;

	// 이미 포함된 멤버인지 체크
	for (const FPartyMember& M : Party.Members)
	{
		if (M.Member == Invitee) return; // 중복 방지
	}

	FPartyMember NewMember;
	NewMember.Member = Invitee;

	if (ATheDSPlayerState* PlayerState = Cast<ATheDSPlayerState>(Invitee))
	{
		NewMember.Nickname = PlayerState->GetNickname();
		NewMember.Level = PlayerState->GetLevel();
		PlayerState->SetPartyLeader(Leader);
	}

	Party.Members.Add(NewMember);

	UE_LOG(LogTemp, Log, TEXT("%s has joined %s's party"),
		*Invitee->GetPlayerName(), *Leader->GetPlayerName());

	UpdateAllPartyMemberStates(Party);
}

void APartyState::LeaveParty(APlayerState* Member)
{
	if (!Member) return;

	ATheDSPlayerState* LeavingPS = Cast<ATheDSPlayerState>(Member);
	if (!LeavingPS || !LeavingPS->GetPartyLeader()) return;

	APlayerState* CurrentLeader = LeavingPS->GetPartyLeader();

	if (!ActiveParties.Contains(CurrentLeader)) return;

	FPartyInfo Party = ActiveParties[CurrentLeader];

	// 멤버 제거
	Party.Members.RemoveAll([=](const FPartyMember& M) {
		return M.Member == Member;
		});

	// PartyLeader 정보 초기화
	LeavingPS->SetPartyLeader(nullptr);

	// 리더 본인이 나간 경우 → 새로운 리더 지정
	if (Member == CurrentLeader)
	{
		if (Party.Members.Num() == 0)
		{
			// 파티 해체
			ActiveParties.Remove(CurrentLeader);
			UE_LOG(LogTemp, Log, TEXT("Party disbanded - no members left."));
			return;
		}

		// 맨 앞의 멤버를 새 리더로 승계
		FPartyMember NewLeaderMember = Party.Members[0];
		APlayerState* NewLeader = NewLeaderMember.Member;

		// 새 리더를 키로 삼아서 파티를 재등록
		ActiveParties.Remove(CurrentLeader);
		Party.Leader = NewLeader;
		ActiveParties.Add(NewLeader, Party);

		// 각 멤버 PartyLeader 갱신
		for (FPartyMember& M : Party.Members)
		{
			if (ATheDSPlayerState* PlayerState = Cast<ATheDSPlayerState>(M.Member))
			{
				PlayerState->SetPartyLeader(NewLeader);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("%s left. New party leader is %s"),
			*Member->GetPlayerName(), *NewLeader->GetPlayerName());
	}
	else
	{
		ActiveParties[CurrentLeader] = Party;
		UE_LOG(LogTemp, Log, TEXT("%s has left the party"), *Member->GetPlayerName());
	}
	UpdateAllPartyMemberStates(Party);
}

FPartyInfo* APartyState::GetParty(APlayerState* Leader)
{
	if (!Leader) return nullptr;
	return ActiveParties.Find(Leader);
}

bool APartyState::IsPartyMember(APlayerState* Player)
{
	for (auto& Elem : ActiveParties)
	{
		for (const FPartyMember& M : Elem.Value.Members)
		{
			if (M.Member == Player) return true;
		}
	}
	return false;
}

void APartyState::UpdateAllPartyMemberStates(const FPartyInfo& Party)
{
	for (const FPartyMember& M : Party.Members)
	{
		if (ATheDSPlayerState* PS = Cast<ATheDSPlayerState>(M.Member))
		{
			PS->SetReplicatedPartyMembers(Party.Members);
		}
	}
}
