#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChattingWidget.generated.h"

UENUM(BlueprintType)
enum class EChatChannel : uint8
{
	Global,
	Party
};

USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Sender;

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	EChatChannel Channel;
};

UCLASS()
class THEDS_API UChattingWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_ChannelLabel;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* ET_Content;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Send;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VB_ContentContainer;

	EChatChannel CurrentChannel;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnChatCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnSendClicked();

	void HandleSendMessage();

public:
	void SetChannel(EChatChannel Channel);

	void ActivateChat();

	void AddChat(const FChatMessage& Message);
};
