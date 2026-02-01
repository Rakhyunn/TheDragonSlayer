#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerGameInstance.h"
#include "LoginRegisterWidget.generated.h"

class UWidgetSwitcher;
class UEditableTextBox;
class UButton;
class UTextBlock;

UCLASS()
class THEDS_API ULoginRegisterWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    /**공통*/
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* PageSwitcher;   // 0: 로그인, 1: 회원가입

    /**로그인 페이지*/
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* Login_IDBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* Login_PWBox;

    UPROPERTY(meta = (BindWidget))
    UButton* Login_LoginBtn;

    UPROPERTY(meta = (BindWidget))
    UButton* Login_GoRegisterBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Login_ResultText;

    /**회원가입 페이지*/
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* Reg_IDBox;

    UPROPERTY(meta = (BindWidget))
    UButton* Reg_CheckIDBtn;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* Reg_PWBox;

    UPROPERTY(meta = (BindWidget))
    UButton* Reg_RegisterBtn;

    UPROPERTY(meta = (BindWidget))
    UButton* Reg_BackToLoginBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Reg_ResultText;

protected:
    virtual void NativeConstruct() override;

    /**버튼 핸들러*/
    UFUNCTION()
    void OnLoginClicked();

    UFUNCTION()
    void OnGoRegisterClicked();

    UFUNCTION()
    void OnRegisterClicked();

    UFUNCTION()
    void OnBackToLoginClicked();

    UFUNCTION()
    void OnCheckIDClicked();

    UFUNCTION()
    void ShowLoginResult(const FString Message);

    UFUNCTION()
    void ShowRegisterResult(const FString Message);

private:
    FTimerHandle LoginResultTimerHandle;

    FTimerHandle RegisterResultTimerHandle;

public:
    UServerGameInstance* GetServerGameInstance(UWorld* World)
    {
        if (!World) return nullptr;
        return World->GetGameInstance<UServerGameInstance>();
    }
};
