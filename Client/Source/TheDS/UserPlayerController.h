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
class URespawnDataAsset;
class ATheDSPlayerState;
class URaidConfirmWidget;
class UEndingConfirmWidget;
class UEndingPlayWidget;
class UEndingBookData;
class URaidGiveUpWidget;
class UNicknameWidget;

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

	bool FindRespawnMap(ATheDSPlayerState* PS, FName& FindVillage, FTransform& FindSpawn) const;

	bool AreAllPartyMembersInVillage(FName RequiredVillage) const;

	void GiveUpRaid();

	UFUNCTION()
	bool IsInRaid() const
	{
		return bIsInRaid;
	}
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Respawn")
	TObjectPtr<URespawnDataAsset> RespawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Raid")
	TSubclassOf<URaidConfirmWidget> RaidConfirmWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ending")
	TSubclassOf<UEndingConfirmWidget> EndingConfirmWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ending")
	TSubclassOf<UEndingPlayWidget> EndingPlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ending")
	TObjectPtr<UEndingBookData> EndingBookData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Raid")
	TSubclassOf<URaidGiveUpWidget> RaidGiveUpWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nickname")
	TSubclassOf<UNicknameWidget> NicknameWidgetClass;

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

	UPROPERTY()
	UNicknameWidget* NicknameWidgetInstance;

	FTimerHandle PortalWarpTimer;

	UPROPERTY(Transient)
	FName CurrentStreamLevel = NAME_None;

	UPROPERTY(Transient)
	int32 PreparedCount = 0;
	UPROPERTY(Transient)
	int32 ExpectedCount = 0;
	UPROPERTY(Transient)
	FString PendingTravelURL;

	UPROPERTY()
	URaidGiveUpWidget* RaidGiveUpWidgetInstance;

	UPROPERTY()
	UEndingPlayWidget* EndingPlayWidgetInstance;

	bool bIsInRaid = false;

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bGiveUpInProgress = false;

public:
	APlayerState* FindNearestPlayer();

	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& NewNickname);

	UFUNCTION(Client, Reliable)
	void ClientOpenShop(class ABaseMerchantNPC* Merchant);

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
	void ServerSystemChat(const FChatMessage& Chat);

	UFUNCTION(Server, Reliable) 
	void ServerSetLastVillage(FName Village, FTransform Spawn);

	UFUNCTION(Server, Reliable) 
	void ServerSetLastField(FName Field);

	UFUNCTION(Server, Reliable)
	void ServerLoadAndWarp(FName LevelName, FTransform Spawn, bool bIsRaid);

	UFUNCTION(Client, Reliable)
	void ClientSetVisibleLevel(FName NewLevelName);

	UFUNCTION(Server, Reliable)
	void ServerShowRaidConfirm(FName LevelName, FTransform Spawn, FName RequiredVillage);

	UFUNCTION(Client, Reliable)
	void ClientShowRaidConfirm(FName LevelName, FTransform Spawn, FName RequiredVillage);

	UFUNCTION(Server, Reliable)
	void ServerRaidConfirmResult(bool bAccept, FName LevelName, FTransform Spawn, FName RequiredVillage);

	UFUNCTION(Client, Reliable)
	void ClientShowGiveUpRaid();

	UFUNCTION(Server, Reliable)
	void ServerRequestGiveUpRaid(bool bAccept);

	UFUNCTION(Server, Reliable)
	void ServerStartRaidInstance(FName BossMap, FName RequiredVillage);

	UFUNCTION(Client, Reliable)
	void ClientPrepareForInstanceTravel();

	UFUNCTION(Server, Reliable)
	void ServerNotifyPreparedForInstance();

	UFUNCTION(Client, Reliable)
	void ClientTravelToBossInstance(const FString& TravelURL);

	UFUNCTION(Client, Reliable)
	void ClientShowEndingConfirm(class ADragonBoss* Dragon, bool bIsLeader);

	UFUNCTION(Server, Reliable)
	void ServerConfirmEnding(class ADragonBoss* Dragon, bool bAccept);

	UFUNCTION(Client, Reliable)
	void ClientPlayEnding(float DurationSec);

	UFUNCTION(Client, Reliable)
	void ClientRemoveEndingWidget();

	void ResetRaid();
};
