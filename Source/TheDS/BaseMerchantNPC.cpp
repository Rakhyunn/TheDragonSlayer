#include "BaseMerchantNPC.h"
#include "UserPlayerController.h"

ABaseMerchantNPC::ABaseMerchantNPC()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseMerchantNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseMerchantNPC::Interact(APlayerController* InteractingPlayer)
{
	if (!InteractingPlayer) return;
	AUserPlayerController* UserPlayer = Cast<AUserPlayerController>(InteractingPlayer);
	if (!UserPlayer) return;
	UserPlayer->OpenShop(this);
}