#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractInterface.h"
#include "BaseMerchantNPC.generated.h"

UENUM(BlueprintType)
enum class EMerchantType : uint8
{
	MT_Consume,
	MT_Equipment
};

UCLASS()
class THEDS_API ABaseMerchantNPC : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ABaseMerchantNPC();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Merchant")
	TArray<class UBaseItem*> ItemsForSale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Merchant")
	EMerchantType MerchantType;

public:
	virtual void Interact(class ABaseCharacter* Interactor) override;

	EMerchantType GetMerchantType() const { return MerchantType; }
	const TArray<UBaseItem*>& GetItemsForSale() const { return ItemsForSale; }
};
