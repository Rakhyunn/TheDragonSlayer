#include "UserPlayerController.h"
#include "PlayerInfoWidget.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerInfoWidget.h"
#include "EquipmentComponent.h"
#include "EquipmentWidget.h"

AUserPlayerController::AUserPlayerController()
{
	SetShowMouseCursor(true);
}

void AUserPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerInfoWidgetClass)
	{
		PlayerInfoWidgetInstance = CreateWidget<UPlayerInfoWidget>(this, PlayerInfoWidgetClass);
		if (PlayerInfoWidgetInstance)
		{
			PlayerInfoWidgetInstance->AddToViewport();

			// 캐릭터에서 스탯 컴포넌트 가져와 바인딩
			APawn* MyPawn = GetPawn();
			ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
			if (MyCharacter && MyCharacter->stat)
			{
				PlayerInfoWidgetInstance->BindInfo(MyCharacter->stat);
			}
		}
	}
}

void AUserPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AUserPlayerController::ToggleInventory);
	InputComponent->BindAction("ToggleEquipment", IE_Pressed, this, &AUserPlayerController::ToggleEquipment);
}

void AUserPlayerController::ToggleInventory()
{
	if (!InventoryWidgetInstance)
	{
		if (!InventoryWidgetClass) return;
		InventoryWidgetInstance = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		UInventoryComponent* inventory = GetPawn()->FindComponentByClass<UInventoryComponent>();
		if (inventory)
		{
			InventoryWidgetInstance->SetInventoryReference(inventory);
		}
	}
	if (InventoryWidgetInstance)
	{
		if (InventoryWidgetInstance->IsInViewport())
		{
			InventoryWidgetInstance->RemoveFromParent();
		}
		else
		{
			ABaseCharacter* player = Cast<ABaseCharacter>(GetPawn());
			InventoryWidgetInstance->SetInventoryReference(player->InventoryComponent);
			InventoryWidgetInstance->AddToViewport();
			InventoryWidgetInstance->RefreshInventory();
		}
	}
}

void AUserPlayerController::ToggleEquipment()
{
	if (!EquipmentWidgetInstance)
	{
		if (!EquipmentWidgetClass) return;
		EquipmentWidgetInstance = CreateWidget<UEquipmentWidget>(this, EquipmentWidgetClass);
		UEquipmentComponent* equipment = GetPawn()->FindComponentByClass<UEquipmentComponent>();
		if (equipment)
		{
			ABaseCharacter* player = Cast<ABaseCharacter>(GetPawn());
			EquipmentWidgetInstance->SetEquipmentReference(equipment, player);
		}
	}
	if (EquipmentWidgetInstance)
	{
		if (EquipmentWidgetInstance->IsInViewport())
		{
			EquipmentWidgetInstance->RemoveFromParent();
		}
		else
		{
			ABaseCharacter* player = Cast<ABaseCharacter>(GetPawn());
			EquipmentWidgetInstance->SetEquipmentReference(player->EquipmentComponent, player);
			EquipmentWidgetInstance->AddToViewport();
			EquipmentWidgetInstance->RefreshEquipment();
		}
	}
}
