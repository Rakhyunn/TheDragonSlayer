#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryComponent.h"
#include "ItemDragDropOperation.generated.h"

UCLASS(BlueprintType)
class THEDS_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
    UPROPERTY()
    class UInventorySlotWidget* SourceSlot;  // 어디서 끌었는지

    UPROPERTY()
    FInventorySlot DraggedItemData;  // 아이템 정보
};
