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
	TArray<FInventorySlot> AllSlots;

	UPROPERTY()
	class UInventoryWidget* linkedInventoryWidget;

public:
	UInventoryComponent();

	void AddItem(UBaseItem* item, int32 quantity);

	void UseItem(int32 index, class ABaseCharacter* target);

	TArray<FInventorySlot> GetFilteredSlots(EItemType filterType) const;
};
