#include "LoginRegisterPlayerController.h"
#include "LoginRegisterWidget.h"

void ALoginRegisterPlayerController::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = true;
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    if (LoginRegisterWidgetClass)
    {
        LoginRegisterWidgetInstance = CreateWidget<ULoginRegisterWidget>(this, LoginRegisterWidgetClass);
        if (LoginRegisterWidgetInstance)
        {
            LoginRegisterWidgetInstance->AddToViewport();
        }
    }
}
