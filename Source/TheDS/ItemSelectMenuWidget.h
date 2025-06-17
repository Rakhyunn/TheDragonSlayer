#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "ItemSelectMenuWidget.generated.h"

UCLASS()
class THEDS_API UItemSelectMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Use;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Drop;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Close;

	FInventorySlot ContextSlot;
	UInventoryComponent* OwningInventory = nullptr;

public:
	virtual void NativeConstruct() override;

	void Setup(FInventorySlot InSlot, UInventoryComponent* InInventory);

	UFUNCTION()
	void OnUseClicked();

	UFUNCTION()
	void OnDropClicked();

	UFUNCTION()
	void OnCloseClicked();
};
