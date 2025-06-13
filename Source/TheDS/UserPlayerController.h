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

public:
	AUserPlayerController();

	void ToggleInventory();
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerInfoWidget> PlayerInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

private:
	UPROPERTY()
	UPlayerInfoWidget* PlayerInfoWidget;

	UPROPERTY()
	UInventoryWidget* InventoryWidgetInstance;
};
