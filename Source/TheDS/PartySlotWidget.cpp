#include "PartySlotWidget.h"
#include "Components/TextBlock.h"

void UPartySlotWidget::Init(const FPartyMember& MemberData, bool bIsLeader)
{
	TXT_Nickname->SetText(FText::FromString(MemberData.Nickname));
	TXT_Level->SetText(FText::Format(FText::FromString("Lv.{0}"), FText::AsNumber(MemberData.Level)));
	if (bIsLeader)
	{
		TXT_Leader->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TXT_Leader->SetVisibility(ESlateVisibility::Collapsed);
	}
}
