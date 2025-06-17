#include "ItemSelectMenuWidget.h"
#include "Components/Button.h"
#include "BaseCharacter.h"

void UItemSelectMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Use) BTN_Use->OnClicked.AddDynamic(this, &UItemSelectMenuWidget::OnUseClicked);
	if (BTN_Drop) BTN_Drop->OnClicked.AddDynamic(this, &UItemSelectMenuWidget::OnDropClicked);
	if (BTN_Close) BTN_Close->OnClicked.AddDynamic(this, &UItemSelectMenuWidget::OnCloseClicked);
}

void UItemSelectMenuWidget::Setup(FInventorySlot InSlot, UInventoryComponent* InInventory)
{
	ContextSlot = InSlot;
	OwningInventory = InInventory;
}

void UItemSelectMenuWidget::OnUseClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Use Clicked"));
	if (OwningInventory)
	{
		APlayerController* PC = GetOwningPlayer();
		ABaseCharacter* player = Cast<ABaseCharacter>(PC->GetPawn());
		if (player)
		{
			OwningInventory->UseItem(ContextSlot.ItemData->itemType, ContextSlot.OriginalIndex, player);
		}
	}
	RemoveFromParent();
}

void UItemSelectMenuWidget::OnDropClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Drop Clicked"));
	if (OwningInventory)
	{
		OwningInventory->RemoveItem(ContextSlot.ItemData->itemType, ContextSlot.OriginalIndex);
	}
	RemoveFromParent();
}

void UItemSelectMenuWidget::OnCloseClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Close Clicked"));
	RemoveFromParent();
}
