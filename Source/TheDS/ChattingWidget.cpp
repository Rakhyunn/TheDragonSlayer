#include "ChattingWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/VerticalBox.h"
#include "UserPlayerController.h"

void UChattingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ET_Content) ET_Content->OnTextCommitted.AddDynamic(this, &UChattingWidget::OnChatCommitted);
	if (BTN_Send) BTN_Send->OnClicked.AddDynamic(this, &UChattingWidget::OnSendClicked);
}

void UChattingWidget::OnChatCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		HandleSendMessage();
	}
}

void UChattingWidget::OnSendClicked()
{
	HandleSendMessage();
}

void UChattingWidget::HandleSendMessage()
{
	FString Message = ET_Content->GetText().ToString();
	if (Message.IsEmpty()) return;
	AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ServerSendChat(Message, CurrentChannel);
	}
	ET_Content->SetText(FText::GetEmpty());
}

void UChattingWidget::SetChannel(EChatChannel Channel)
{
	CurrentChannel = Channel;
	if (TXT_ChannelLabel)
	{
		FString Label = (Channel == EChatChannel::Global) ? TEXT("[전체]") : TEXT("[파티]");
		TXT_ChannelLabel->SetText(FText::FromString(Label));
	}
}

void UChattingWidget::ActivateChat()
{
	SetVisibility(ESlateVisibility::Visible);
	if(ET_Content) ET_Content->SetKeyboardFocus();
}

void UChattingWidget::AddChat(const FChatMessage& Chat)
{
	UTextBlock* Line = NewObject<UTextBlock>(this);
	Line->SetText(FText::FromString(FString::Printf(TEXT("[%s] %s"), *Chat.Sender, *Chat.Message)));
	// 색상 설정
	if (Chat.Channel == EChatChannel::Party)
	{
		Line->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	else
	{
		Line->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
	VB_ContentContainer->AddChild(Line);
}
