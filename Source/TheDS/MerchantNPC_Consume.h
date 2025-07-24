#pragma once

#include "CoreMinimal.h"
#include "BaseMerchantNPC.h"
#include "MerchantNPC_Consume.generated.h"

UCLASS()
class THEDS_API AMerchantNPC_Consume : public ABaseMerchantNPC
{
	GENERATED_BODY()
	
public:
	AMerchantNPC_Consume() { MerchantType = EMerchantType::MT_Consume; }
};
