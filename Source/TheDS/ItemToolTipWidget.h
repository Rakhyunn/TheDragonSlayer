#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTipWidget.generated.h"

UCLASS()
class THEDS_API UItemToolTipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_Icon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Description;

public:
	void InitTooltip(class UBaseItem* ItemData);
};
