#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvitePartyWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class THEDS_API UInvitePartyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_LeaderName;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Accept;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Decline;

	APlayerState* Leader;

protected:
	virtual void NativeConstruct() override;

public:
	void Init(APlayerState* FromLeader);

	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnDeclineClicked();
};
