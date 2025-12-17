#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingBookData.h"
#include "EndingPlayWidget.generated.h"

UCLASS()
class THEDS_API UEndingPlayWidget : public UUserWidget
{
	GENERATED_BODY()

private:
    UPROPERTY(Transient)
    TObjectPtr<UEndingBookData> Data;
    TArray<FEndingPage> Pages;
    int32 PageIndex = 0;

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TXT_Title;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TXT_Body;

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Prev;

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Next;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TXT_PageIndicator;
	
protected:
    virtual void NativeConstruct() override;

public:
    UFUNCTION()
    void InitData(class UEndingBookData* InData);

protected:
    UFUNCTION()
    void OnPrev();

    UFUNCTION()
    void OnNext();

    void RefreshPage();

    void UpdateNav();
};
