#include "DropItemActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "InventoryComponent.h"

ADropItemActor::ADropItemActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->InitSphereRadius(80.f);
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(FName("PickUp"));
}

void ADropItemActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeTime);
	UE_LOG(LogTemp, Warning, TEXT("DropItemActor Spawned"));
}

void ADropItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADropItemActor::Init(UBaseItem* itemData, int32 quantity)
{
	ItemData = itemData;
	Quantity = quantity;
	if (ItemData && ItemData->DroppedMesh)
	{
		Mesh->SetStaticMesh(ItemData->DroppedMesh);
	}
}

void ADropItemActor::Interact(ABaseCharacter* Interactor)
{
	if (Interactor && ItemData)
	{
		Interactor->InventoryComponent->AddItem(ItemData, Quantity);
		Destroy();
	}
}

