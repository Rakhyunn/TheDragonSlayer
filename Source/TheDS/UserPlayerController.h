#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChattingWidget.h"
#include "UserPlayerController.generated.h"

class UPlayerInfoWidget;
class UBaseStatComponent;
class UInventoryWidget;
class UEquipmentWidget;
class UNPCShopWidget;
class UInvitePartyWidget;

UCLASS()
class THEDS_API AUserPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUserPlayerController();

	void ToggleInventory();

	void ToggleEquipment();

	void OpenShop(class ABaseMerchantNPC* Merchant);

	void OpenChatInput();

	void SwitchToGlobalChat();

	void SwitchToPartyChat();

	void UsePortal(class APortalActor* Portal);
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerInfoWidget> PlayerInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEquipmentWidget> EquipmentWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Merchant")
	TSubclassOf<UNPCShopWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Party")
	TSubclassOf<UInvitePartyWidget> InvitePartyWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chat")
	TSubclassOf<UChattingWidget> ChattingWidgetClass;

private:
	UPROPERTY()
	UPlayerInfoWidget* PlayerInfoWidgetInstance;

	UPROPERTY()
	UInventoryWidget* InventoryWidgetInstance;

	UPROPERTY()
	UEquipmentWidget* EquipmentWidgetInstance;

	UPROPERTY()
	UNPCShopWidget* ShopWidgetInstance;

	UPROPERTY()
	UChattingWidget* ChattingWidgetInstance;

	FTimerHandle PortalWarpTimer;

public:
	APlayerState* FindNearestPlayer();

	UFUNCTION(Server, Reliable)
	void ServerRequestCreateParty();
	UFUNCTION(Server, Reliable)
	void ServerRequestInviteParty(APlayerState* Invitee);
	UFUNCTION(Server, Reliable)
	void ServerRequestLeaveParty();

	void RequestCreateParty();
	void RequestInviteParty(APlayerState* Invitee);
	void RequestLeaveParty();

	UFUNCTION(Client, Reliable)
	void ClientShowPartyInvite(APlayerState* FromLeader);

	UFUNCTION(Server, Reliable)
	void ServerRespondToInvite(bool bAccepted, APlayerState* FromLeader);

	UFUNCTION(Client, Reliable)
	void ClientUpdatePartyUI();

	UFUNCTION(Server, Reliable)
	void ServerSendChat(const FString& Message, EChatChannel Channel);

	UFUNCTION(Client, Reliable)
	void ClientReceiveChat(const FChatMessage& Chat);

	UFUNCTION(Server, Reliable)
	void ServerLoadAndWarp(FName LevelName, FTransform Spawn, bool bIsRaid);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLoadAndWarp(FName LevelName);

	UFUNCTION(Client, Reliable)
	void ClientShowRaidConfirm(FName LevelName, FTransform Spawn);

	UFUNCTION(Server, Reliable)
	void ServerRaidConfirmResult(bool bAccept, FName LevelName, FTransform Spawn);
};
