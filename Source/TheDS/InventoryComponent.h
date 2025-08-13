#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseItem.h"
#include "Item_Equipment.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBaseItem* ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEquipped = false;

	int32 OriginalIndex = -1;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THEDS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> ConsumeSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> EquipmentSlots;

	UPROPERTY()
	class UInventoryWidget* linkedInventoryWidget;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxSlotCount = 30;

public:
	UInventoryComponent();

	void AddItem(UBaseItem* Item, int32 Quantity);

	void UseItem(EItemType Type, int32 Index, class ABaseCharacter* Target);

	void SwapItem(EItemType Type, int32 FromIndex, int32 ToIndex);

	void RemoveItem(EItemType Type, int32 Index);

	const TArray<FInventorySlot>& GetSlots(EItemType Type) const;
};
