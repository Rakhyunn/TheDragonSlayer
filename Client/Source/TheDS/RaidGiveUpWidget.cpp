#include "RaidGiveUpWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UserPlayerController.h"

void URaidGiveUpWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Accept) BTN_Accept->OnClicked.AddDynamic(this, &URaidGiveUpWidget::OnAcceptClicked);
	if (BTN_Decline) BTN_Decline->OnClicked.AddDynamic(this, &URaidGiveUpWidget::OnDeclineClicked);
}

void URaidGiveUpWidget::Init(FName LevelName)
{
	if (LevelName == FName("WitchField"))
	{
		if (TXT_RaidType)
		{
			TXT_RaidType->SetText(FText::FromString("Witch Raid"));
		}
	}
	else if (LevelName == FName("DragonField"))
	{
		if (TXT_RaidType)
		{
			TXT_RaidType->SetText(FText::FromString("Dragon Raid"));
		}
	}
}

void URaidGiveUpWidget::OnAcceptClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRequestGiveUpRaid(true);
		RemoveFromParent();
	}
}

void URaidGiveUpWidget::OnDeclineClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRequestGiveUpRaid(false);
		if (BTN_Accept) BTN_Accept->OnClicked.RemoveAll(this);
		if (BTN_Decline) BTN_Decline->OnClicked.RemoveAll(this);
		RemoveFromParent();
	}
}
