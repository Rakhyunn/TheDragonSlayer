#include "LoginRegisterWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void ULoginRegisterWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // 로그인 페이지 버튼 바인딩
    if (Login_LoginBtn)
    {
        Login_LoginBtn->OnClicked.AddDynamic(this, &ULoginRegisterWidget::OnLoginClicked);
    }
    if (Login_GoRegisterBtn)
    {
        Login_GoRegisterBtn->OnClicked.AddDynamic(this, &ULoginRegisterWidget::OnGoRegisterClicked);
    }
    // 회원가입 페이지 버튼 바인딩
    if (Reg_BackToLoginBtn)
    {
        Reg_BackToLoginBtn->OnClicked.AddDynamic(this, &ULoginRegisterWidget::OnBackToLoginClicked);
    }
    if (Reg_RegisterBtn)
    {
        Reg_RegisterBtn->OnClicked.AddDynamic(this, &ULoginRegisterWidget::OnRegisterClicked);
    }
    if (Reg_CheckIDBtn)
    {
        Reg_CheckIDBtn->OnClicked.AddDynamic(this, &ULoginRegisterWidget::OnCheckIDClicked);
    }
    if (Reg_CheckNickBtn)
    {
        Reg_CheckNickBtn->OnClicked.AddDynamic(this, &ULoginRegisterWidget::OnCheckNickClicked);
    }
    // 처음엔 로그인 페이지 활성
    if (PageSwitcher)
    {
        PageSwitcher->SetActiveWidgetIndex(0);
    }
    // 결과 텍스트 초기화
    if (Login_ResultText)
    {
        Login_ResultText->SetText(FText::GetEmpty());
    }
    if (Reg_ResultText)
    {
        Reg_ResultText->SetText(FText::GetEmpty());
    }
}

void ULoginRegisterWidget::OnLoginClicked()
{
    if (!Login_IDBox || !Login_PWBox || !Login_ResultText) return;
    if (Login_IDBox->GetText().IsEmpty() || Login_PWBox->GetText().IsEmpty())
    {
        FString DebugMsg = FString::Printf(TEXT("Please Put All Contents"));
        ShowLoginResult(DebugMsg);
        return;
    }
    const FString Id = Login_IDBox->GetText().ToString();
    const FString Pw = Login_PWBox->GetText().ToString();
    UServerGameInstance* GI = GetServerGameInstance(GetWorld());
    if (!GI)
    {
        FString DebugMsg = FString::Printf(TEXT("Please Check GameInstance"));
        ShowLoginResult(DebugMsg);
        return;
    }
    const FString Result = GI->LoginAccount(Id, Pw);
    ShowLoginResult(Result);
    if (Result == TEXT("Login Success"))
    {
        RemoveFromParent();
        GetWorld()->GetTimerManager().ClearTimer(LoginResultTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(RegisterResultTimerHandle);
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
            PC->ClientTravel(TEXT("127.0.0.1:7777"), TRAVEL_Absolute);
        }
        //FName NextLevel = TEXT("TestMap");
        //UGameplayStatics::OpenLevel(this, NextLevel);
    }
}

void ULoginRegisterWidget::OnGoRegisterClicked()
{
    if (PageSwitcher)
    {
        PageSwitcher->SetActiveWidgetIndex(1);
        Login_IDBox->SetText(FText::GetEmpty());
        Login_PWBox->SetText(FText::GetEmpty());
    }
}

void ULoginRegisterWidget::OnBackToLoginClicked()
{
    if (PageSwitcher)
    {
        PageSwitcher->SetActiveWidgetIndex(0);
        Reg_IDBox->SetText(FText::GetEmpty());
        Reg_PWBox->SetText(FText::GetEmpty());
        Reg_NickBox->SetText(FText::GetEmpty());
    }
}

void ULoginRegisterWidget::OnRegisterClicked()
{
    if (!Reg_IDBox || !Reg_PWBox || !Reg_NickBox || !Reg_ResultText) return;
    if (Reg_IDBox->GetText().IsEmpty() || Reg_PWBox->GetText().IsEmpty() || Reg_NickBox->GetText().IsEmpty())
    {
        FString DebugMsg = FString::Printf(TEXT("Please Make All Contents"));
        ShowRegisterResult(DebugMsg);
        return;
    }
    const FString Id = Reg_IDBox->GetText().ToString();
    const FString Pw = Reg_PWBox->GetText().ToString();
    const FString Nick = Reg_NickBox->GetText().ToString();
    UServerGameInstance* GI = GetServerGameInstance(GetWorld());
    if (!GI)
    {
        FString DebugMsg = FString::Printf(TEXT("Please Check GameInstance"));
        ShowRegisterResult(DebugMsg);
        return;
    }
    const FString Result = GI->RegisterAccount(Id, Pw, Nick);
    ShowRegisterResult(Result);
    if (Result.Contains("Success"))
    {
        Reg_IDBox->SetText(FText::GetEmpty());
        Reg_PWBox->SetText(FText::GetEmpty());
        Reg_NickBox->SetText(FText::GetEmpty());
    }
}

void ULoginRegisterWidget::OnCheckIDClicked()
{
    if (!Reg_IDBox || !Reg_ResultText) return;
    if (Reg_IDBox->GetText().IsEmpty())
    {
        FString DebugMsg = FString::Printf(TEXT("Please Put Your Own ID"));
        ShowRegisterResult(DebugMsg);
        return;
    }
    const FString Id = Reg_IDBox->GetText().ToString();
    UServerGameInstance* GI = GetServerGameInstance(GetWorld());
    if (!GI)
    {
        FString DebugMsg = FString::Printf(TEXT("Please Check GameInstance"));
        ShowRegisterResult(DebugMsg);
        return;
    }
    const FString Result = GI->CheckID(Id);
    ShowRegisterResult(Result);
}

void ULoginRegisterWidget::OnCheckNickClicked()
{
    if (!Reg_NickBox || !Reg_ResultText) return;
    if (Reg_NickBox->GetText().IsEmpty())
    {
        FString DebugMsg = FString::Printf(TEXT("Please Put Your Own Nickname"));
        ShowRegisterResult(DebugMsg);
        return;
    }
    const FString Nick = Reg_NickBox->GetText().ToString();
    UServerGameInstance* GI = GetServerGameInstance(GetWorld());
    if (!GI)
    {
        FString DebugMsg = FString::Printf(TEXT("Please Check GameInstance"));
        ShowRegisterResult(DebugMsg);
        return;
    }
    const FString Result = GI->CheckNickname(Nick);
    ShowRegisterResult(Result);
}

void ULoginRegisterWidget::ShowLoginResult(const FString Message)
{
    GetWorld()->GetTimerManager().ClearTimer(LoginResultTimerHandle);
    if (Login_ResultText)
    {
        Login_ResultText->SetText(FText::FromString(Message));
    }
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda([this]()
        {
            if (Login_ResultText)
            {
                Login_ResultText->SetText(FText::GetEmpty());
            }
        });
    GetWorld()->GetTimerManager().SetTimer(
        LoginResultTimerHandle,
        TimerDelegate,
        3.f,
        false
    );
}

void ULoginRegisterWidget::ShowRegisterResult(const FString Message)
{
    GetWorld()->GetTimerManager().ClearTimer(RegisterResultTimerHandle);
    if (Reg_ResultText)
    {
        Reg_ResultText->SetText(FText::FromString(Message));
    }
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda([this]()
        {
            if (Reg_ResultText)
            {
                Reg_ResultText->SetText(FText::GetEmpty());
            }
        });
    GetWorld()->GetTimerManager().SetTimer(
        RegisterResultTimerHandle,
        TimerDelegate,
        3.f,
        false
    );
}
