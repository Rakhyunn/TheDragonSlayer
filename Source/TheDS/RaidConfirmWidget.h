#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaidConfirmWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class THEDS_API URaidConfirmWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_RaidType;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Accept;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Decline;

	FName LevelName;
	FTransform Spawn;
	FName RequiredVillage;

protected:
	virtual void NativeConstruct() override;

public:
	void Init(FName LevelName_, FTransform Spawn_, FName RequiredVillage_);

	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnDeclineClicked();
};
