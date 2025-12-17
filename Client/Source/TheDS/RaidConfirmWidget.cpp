#include "RaidConfirmWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UserPlayerController.h"

void URaidConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Accept) BTN_Accept->OnClicked.AddDynamic(this, &URaidConfirmWidget::OnAcceptClicked);
	if (BTN_Decline) BTN_Decline->OnClicked.AddDynamic(this, &URaidConfirmWidget::OnDeclineClicked);
}

void URaidConfirmWidget::Init(FName LevelName_, FTransform Spawn_, FName RequiredVillage_)
{
	LevelName = LevelName_;
	Spawn = Spawn_;
	RequiredVillage = RequiredVillage_;
	if (LevelName_ == FName("WitchField"))
	{
		if (TXT_RaidType)
		{
			TXT_RaidType->SetText(FText::FromString("Witch Raid"));
		}
	}
	else if (LevelName_ == FName("DragonField"))
	{
		if (TXT_RaidType)
		{
			TXT_RaidType->SetText(FText::FromString("Dragon Raid"));
		}
	}
}

void URaidConfirmWidget::OnAcceptClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRaidConfirmResult(true, LevelName, Spawn, RequiredVillage);
		RemoveFromParent();
	}
}

void URaidConfirmWidget::OnDeclineClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRaidConfirmResult(false, LevelName, Spawn, RequiredVillage);
		RemoveFromParent();
	}
}
