#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaidGiveUpWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class THEDS_API URaidGiveUpWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_RaidType;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Accept;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Decline;

protected:
	virtual void NativeConstruct() override;

public:
	void Init(FName LevelName);

	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnDeclineClicked();
};
