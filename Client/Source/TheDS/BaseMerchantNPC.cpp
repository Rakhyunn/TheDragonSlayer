#include "BaseMerchantNPC.h"
#include "UserPlayerController.h"
#include "BaseCharacter.h"

ABaseMerchantNPC::ABaseMerchantNPC()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseMerchantNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseMerchantNPC::Interact(ABaseCharacter* Interactor)
{
	if (!Interactor) return;

	AUserPlayerController* UserPlayer = Cast<AUserPlayerController>(Interactor->GetController());
	if (UserPlayer)
	{
		UserPlayer->ClientOpenShop(this);
	}
}
