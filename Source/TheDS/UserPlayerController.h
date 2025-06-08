#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UserPlayerController.generated.h"

class UPlayerInfoWidget;
class UBaseStatComponent;

UCLASS()
class THEDS_API AUserPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerInfoWidget> PlayerInfoWidgetClass;

private:
	UPROPERTY()
	UPlayerInfoWidget* PlayerInfoWidget;
};
