#include "UserPlayerController.h"
#include "PlayerInfoWidget.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerInfoWidget.h"
#include "EquipmentComponent.h"
#include "EquipmentWidget.h"
#include "BaseMerchantNPC.h"
#include "NPCShopWidget.h"
#include "PartyState.h"
#include "TheDSPlayerState.h"
#include "Engine/World.h"
#include "InvitePartyWidget.h"

AUserPlayerController::AUserPlayerController()
{
	SetShowMouseCursor(true);
}

void AUserPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerInfoWidgetClass)
	{
		PlayerInfoWidgetInstance = CreateWidget<UPlayerInfoWidget>(this, PlayerInfoWidgetClass);
		if (PlayerInfoWidgetInstance)
		{
			PlayerInfoWidgetInstance->AddToViewport();

			// 캐릭터에서 스탯 컴포넌트 가져와 바인딩
			APawn* MyPawn = GetPawn();
			ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
			if (MyCharacter && MyCharacter->stat)
			{
				PlayerInfoWidgetInstance->BindInfo(MyCharacter->stat);
				PlayerInfoWidgetInstance->UpdateParty();
			}
		}
	}
}

void AUserPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AUserPlayerController::ToggleInventory);
	InputComponent->BindAction("ToggleEquipment", IE_Pressed, this, &AUserPlayerController::ToggleEquipment);
}

void AUserPlayerController::ServerRequestCreateParty_Implementation()
{
	RequestCreateParty();
}

void AUserPlayerController::ServerRequestInviteParty_Implementation(APlayerState* Invitee)
{
	RequestInviteParty(Invitee);
}

void AUserPlayerController::ServerRequestLeaveParty_Implementation()
{
	RequestLeaveParty();
}

APlayerState* AUserPlayerController::FindNearestPlayer()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return nullptr;
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	APlayerState* Nearest = nullptr;
	float MinDistSq = TNumericLimits<float>::Max();
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* OtherPC = It->Get();
		if (!OtherPC || OtherPC == this) continue;
		APawn* OtherPawn = OtherPC->GetPawn();
		if (!OtherPawn) continue;
		APlayerState* OtherState = OtherPC->PlayerState;
		if (!OtherState || OtherState == PlayerState) continue;
		// 이미 파티에 있으면 제외
		ATheDSPlayerState* OtherDS = Cast<ATheDSPlayerState>(OtherState);
		if (OtherDS && OtherDS->IsInParty()) continue;
		float DistSq = FVector::DistSquared(MyPawn->GetActorLocation(), OtherPawn->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Nearest = OtherState;
		}
	}
	return Nearest;
}

void AUserPlayerController::RequestCreateParty()
{
	if (HasAuthority())
	{
		if (APartyState* PartyState = GetWorld()->GetGameState<APartyState>())
		{
			PartyState->CreateParty(PlayerState);
		}
	}
	else
	{
		ServerRequestCreateParty();
	}
}

void AUserPlayerController::RequestInviteParty(APlayerState* Invitee)
{
	if (!Invitee) return;

	if (HasAuthority())
	{
		if (APartyState* PartyState = GetWorld()->GetGameState<APartyState>())
		{
			PartyState->InviteToParty(PlayerState, Invitee);
		}
	}
	else
	{
		ServerRequestInviteParty(Invitee);
	}
}

void AUserPlayerController::RequestLeaveParty()
{
	if (HasAuthority())
	{
		if (APartyState* PartyState = GetWorld()->GetGameState<APartyState>())
		{
			PartyState->LeaveParty(PlayerState);
		}
	}
	else
	{
		ServerRequestLeaveParty();
	}
}

void AUserPlayerController::ClientUpdatePartyUI_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ClientUpdatePartyUI called: %s"), PlayerInfoWidgetInstance ? TEXT("Widget OK") : TEXT("Widget is NULL"));
	if (PlayerInfoWidgetInstance)
	{
		PlayerInfoWidgetInstance->UpdateParty();
	}
}

void AUserPlayerController::ClientShowPartyInvite_Implementation(APlayerState* FromLeader)
{
	if (!InvitePartyWidgetClass) return;
	UInvitePartyWidget* InviteWidget = CreateWidget<UInvitePartyWidget>(this, InvitePartyWidgetClass);
	if (InviteWidget)
	{
		InviteWidget->Init(FromLeader); // FromLeader 정보 UI에 표시
		InviteWidget->AddToViewport();
	}
}

void AUserPlayerController::ServerRespondToInvite_Implementation(bool bAccepted, APlayerState* FromLeader)
{
	if(!FromLeader || !PlayerState) return;
	if (bAccepted)
	{
		if (APartyState* PartyState = GetWorld()->GetGameState<APartyState>())
		{
			PartyState->AcceptInvite(PlayerState, FromLeader); // 리더 기준으로 자신 추가
		}
	}
	else
	{
		// 거절 시 행동 필요하면 여기에
		UE_LOG(LogTemp, Warning, TEXT("%s declined the party invite from %s"),
			*PlayerState->GetPlayerName(),
			*FromLeader->GetPlayerName());
	}
}

void AUserPlayerController::ToggleInventory()
{
	if (!InventoryWidgetInstance)
	{
		if (!InventoryWidgetClass) return;
		InventoryWidgetInstance = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		UInventoryComponent* inventory = GetPawn()->FindComponentByClass<UInventoryComponent>();
		if (inventory)
		{
			InventoryWidgetInstance->SetInventoryReference(inventory);
		}
	}
	if (InventoryWidgetInstance)
	{
		if (InventoryWidgetInstance->IsInViewport())
		{
			InventoryWidgetInstance->RemoveFromParent();
		}
		else
		{
			ABaseCharacter* player = Cast<ABaseCharacter>(GetPawn());
			InventoryWidgetInstance->SetInventoryReference(player->InventoryComponent);
			InventoryWidgetInstance->AddToViewport();
			InventoryWidgetInstance->RefreshInventory();
		}
	}
}

void AUserPlayerController::ToggleEquipment()
{
	if (!EquipmentWidgetInstance)
	{
		if (!EquipmentWidgetClass) return;
		EquipmentWidgetInstance = CreateWidget<UEquipmentWidget>(this, EquipmentWidgetClass);
		UEquipmentComponent* equipment = GetPawn()->FindComponentByClass<UEquipmentComponent>();
		if (equipment)
		{
			ABaseCharacter* player = Cast<ABaseCharacter>(GetPawn());
			EquipmentWidgetInstance->SetEquipmentReference(equipment, player);
		}
	}
	if (EquipmentWidgetInstance)
	{
		if (EquipmentWidgetInstance->IsInViewport())
		{
			EquipmentWidgetInstance->RemoveFromParent();
		}
		else
		{
			ABaseCharacter* player = Cast<ABaseCharacter>(GetPawn());
			EquipmentWidgetInstance->SetEquipmentReference(player->EquipmentComponent, player);
			EquipmentWidgetInstance->AddToViewport();
			EquipmentWidgetInstance->RefreshEquipment();
		}
	}
}

void AUserPlayerController::OpenShop(ABaseMerchantNPC* Merchant)
{
	if (!ShopWidgetClass || !Merchant) return;

	if (!ShopWidgetInstance)
	{
		ShopWidgetInstance = CreateWidget<UNPCShopWidget>(this, ShopWidgetClass);
	}

	if (ShopWidgetInstance && !ShopWidgetInstance->IsInViewport())
	{
		ShopWidgetInstance->InitShop(Merchant); // 아이템 설정 등
		ShopWidgetInstance->AddToViewport();
	}
}