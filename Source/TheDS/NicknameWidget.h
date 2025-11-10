#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NicknameWidget.generated.h"

UCLASS()
class THEDS_API UNicknameWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ET_Nickname;

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Confirm;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnConfirmClicked();
};
