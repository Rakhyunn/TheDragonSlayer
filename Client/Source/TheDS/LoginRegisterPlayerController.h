#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoginRegisterPlayerController.generated.h"

class ULoginRegisterWidget;

UCLASS()
class THEDS_API ALoginRegisterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULoginRegisterWidget> LoginRegisterWidgetClass;

	UPROPERTY()
	ULoginRegisterWidget* LoginRegisterWidgetInstance;

protected:
	virtual void BeginPlay() override;
};
