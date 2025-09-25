#include "EndingConfirmWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UserPlayerController.h"
#include "DragonBoss.h"

void UEndingConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Accept) BTN_Accept->OnClicked.AddDynamic(this, &UEndingConfirmWidget::OnAcceptClicked);
	if (BTN_Decline) BTN_Decline->OnClicked.AddDynamic(this, &UEndingConfirmWidget::OnDeclineClicked);
}

void UEndingConfirmWidget::Init(class ADragonBoss* Dragon_, bool bIsLeader_)
{
	Dragon = Dragon_;
	bIsLeader = bIsLeader_;
	if (TXT_Info)
	{
		TXT_Info->SetText(
			FText::FromString(bIsLeader ? TEXT("엔딩을 수락하시겠습니까?") 
				: TEXT("파티장이 엔딩을 수락하면 진행됩니다.")));
	}
	if (BTN_Accept) BTN_Accept->SetIsEnabled(bIsLeader);
	if (BTN_Decline) BTN_Decline->SetIsEnabled(bIsLeader);
}

void UEndingConfirmWidget::OnAcceptClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerConfirmEnding(Dragon, true);
		RemoveFromParent();
	}
}

void UEndingConfirmWidget::OnDeclineClicked()
{
	if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
	{
		PC->ServerConfirmEnding(Dragon, false);
		RemoveFromParent();
	}
}
