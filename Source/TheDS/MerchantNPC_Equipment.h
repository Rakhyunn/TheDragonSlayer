#pragma once

#include "CoreMinimal.h"
#include "BaseMerchantNPC.h"
#include "MerchantNPC_Equipment.generated.h"

UCLASS()
class THEDS_API AMerchantNPC_Equipment : public ABaseMerchantNPC
{
	GENERATED_BODY()
	
public:
	AMerchantNPC_Equipment() { MerchantType = EMerchantType::MT_Equipment; }
};
