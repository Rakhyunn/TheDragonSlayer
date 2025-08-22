#include "PortalActor.h"
#include "Components/BoxComponent.h"
#include "BaseCharacter.h"
#include "UserPlayerController.h"

APortalActor::APortalActor()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Tags.Add("Portal");
}

void APortalActor::BeginPlay()
{
	Super::BeginPlay();
}

void APortalActor::Interact(ABaseCharacter* Interactor)
{
	if (!Interactor) return;

	AUserPlayerController* UserPlayer = Cast<AUserPlayerController>(Interactor->GetController());
	if (!UserPlayer) return;
	UserPlayer->UsePortal(this);
}

