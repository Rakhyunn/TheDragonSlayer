#include "InvitePartyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UserPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "TheDSPlayerState.h"

void UInvitePartyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Accept) BTN_Accept->OnClicked.AddDynamic(this, &UInvitePartyWidget::OnAcceptClicked);
	if (BTN_Decline) BTN_Decline->OnClicked.AddDynamic(this, &UInvitePartyWidget::OnDeclineClicked);
}

void UInvitePartyWidget::Init(APlayerState* FromLeader)
{
	Leader = FromLeader;
	if (ATheDSPlayerState* PS = Cast<ATheDSPlayerState>(Leader))
	{
		TXT_LeaderName->SetText(FText::FromString(PS->GetNickname()));
	}
	else
	{
		TXT_LeaderName->SetText(FText::FromString(Leader->GetPlayerName()));
	}
}

void UInvitePartyWidget::OnAcceptClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRespondToInvite(true, Leader);
		RemoveFromParent();
	}
}

void UInvitePartyWidget::OnDeclineClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRespondToInvite(false, Leader);
		RemoveFromParent();
	}
}
