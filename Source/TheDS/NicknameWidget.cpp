#include "NicknameWidget.h"
#include "UserPlayerController.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UNicknameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Confirm)BTN_Confirm->OnClicked.AddDynamic(this, &UNicknameWidget::OnConfirmClicked);
}

void UNicknameWidget::OnConfirmClicked()
{
    const FString Nick = ET_Nickname->GetText().ToString();
    if (Nick.IsEmpty()) return;
    if (AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer()))
    {
        PC->ServerSetNickname(Nick);
        RemoveFromParent();
    }
}
