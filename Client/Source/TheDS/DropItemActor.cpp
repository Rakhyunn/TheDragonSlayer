#include "DropItemActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

ADropItemActor::ADropItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bOnlyRelevantToOwner = true;
	bNetUseOwnerRelevancy = true;

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

void ADropItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADropItemActor, RepMesh);
	DOREPLIFETIME(ADropItemActor, RepQuantity);
}

void ADropItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADropItemActor::Init(UBaseItem* ItemData_, int32 Quantity_)
{
	ItemData = ItemData_;
	Quantity = Quantity_;
	RepQuantity = Quantity_;
	if (ItemData && ItemData->DroppedMesh)
	{
		RepMesh = ItemData->DroppedMesh;
		Mesh->SetStaticMesh(RepMesh.LoadSynchronous());
	}
}

void ADropItemActor::Interact(ABaseCharacter* Interactor)
{
	if (!HasAuthority()) return;
	if (Interactor && ItemData)
	{
		Interactor->InventoryComponent->AddItem(ItemData, Quantity);
		Destroy();
	}
}

void ADropItemActor::OnRep_Setup()
{
	if (RepMesh.IsValid())
	{
		Mesh->SetStaticMesh(RepMesh.Get());
	}
}

