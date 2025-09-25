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
#include "PortalActor.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RespawnDataAsset.h"
#include "EnemySpawnManager.h"
#include "RaidConfirmWidget.h"
#include "DSGameMode.h"
#include "DragonBoss.h"
#include "EndingConfirmWidget.h"
#include "EndingPlayWidget.h"
#include "EndingBookData.h"

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
			if (MyCharacter && MyCharacter->Stat)
			{
				PlayerInfoWidgetInstance->BindInfo(MyCharacter->Stat);
				PlayerInfoWidgetInstance->UpdateParty();
			}
		}
	}

	if (ChattingWidgetClass)
	{
		ChattingWidgetInstance = CreateWidget<UChattingWidget>(this, ChattingWidgetClass);
		if (ChattingWidgetInstance)
		{
			ChattingWidgetInstance->AddToViewport();
			ChattingWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			ChattingWidgetInstance->SetPositionInViewport(FVector2D(100, 200));
		}
	}
}

void AUserPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AUserPlayerController::ToggleInventory);
	InputComponent->BindAction("ToggleEquipment", IE_Pressed, this, &AUserPlayerController::ToggleEquipment);

	InputComponent->BindAction("OpenChat", IE_Pressed, this, &AUserPlayerController::OpenChatInput);
	InputComponent->BindAction("SwitchToGlobalChat", IE_Pressed, this, &AUserPlayerController::SwitchToGlobalChat);
	InputComponent->BindAction("SwitchToPartyChat", IE_Pressed, this, &AUserPlayerController::SwitchToPartyChat);
}

void AUserPlayerController::ToggleInventory()
{
	if (!InventoryWidgetInstance)
	{
		if (!InventoryWidgetClass) return;
		InventoryWidgetInstance = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		UInventoryComponent* Inventory = GetPawn()->FindComponentByClass<UInventoryComponent>();
		if (Inventory)
		{
			InventoryWidgetInstance->SetInventoryReference(Inventory);
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
			ABaseCharacter* Player_ = Cast<ABaseCharacter>(GetPawn());
			InventoryWidgetInstance->SetInventoryReference(Player_->InventoryComponent);
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
		UEquipmentComponent* Equipment = GetPawn()->FindComponentByClass<UEquipmentComponent>();
		if (Equipment)
		{
			ABaseCharacter* Player_ = Cast<ABaseCharacter>(GetPawn());
			EquipmentWidgetInstance->SetEquipmentReference(Equipment, Player_);
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
			ABaseCharacter* Player_ = Cast<ABaseCharacter>(GetPawn());
			EquipmentWidgetInstance->SetEquipmentReference(Player_->EquipmentComponent, Player_);
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
		ShopWidgetInstance->InitShop(Merchant);
		ShopWidgetInstance->AddToViewport();
	}
}

void AUserPlayerController::OpenChatInput()
{
	if (ChattingWidgetInstance)
	{
		ChattingWidgetInstance->ActivateChat();
	}
}

void AUserPlayerController::SwitchToGlobalChat()
{
	if (ChattingWidgetInstance)
	{
		ChattingWidgetInstance->SetChannel(EChatChannel::Global);
		ChattingWidgetInstance->ActivateChat();
	}
}

void AUserPlayerController::SwitchToPartyChat()
{
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (PS && !PS->IsInParty())
	{
		FChatMessage ErrorMsg;
		ErrorMsg.Sender = TEXT("시스템");
		ErrorMsg.Message = TEXT("파티에 속해있지 않습니다.");
		ErrorMsg.Channel = EChatChannel::Global;
		ServerSystemChat(ErrorMsg);
		return;
	}
	if (ChattingWidgetInstance)
	{
		ChattingWidgetInstance->SetChannel(EChatChannel::Party);
		ChattingWidgetInstance->ActivateChat();
	}
}

void AUserPlayerController::UsePortal(APortalActor* Portal)
{
	if (!Portal) return;
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	
	switch (Portal->GetPortalType())
	{
	case EPortalType::Village:
		if (PS) ServerSetLastVillage(Portal->GetTargetMap(), Portal->GetTargetTransform());
		if (!Portal->GetTargetMap().IsNone())
		{
			ServerLoadAndWarp(Portal->GetTargetMap(), Portal->GetTargetTransform(), false);
		}
		break;
	case EPortalType::MonsterField:
		if (PS) ServerSetLastField(Portal->GetTargetMap());
		if (!Portal->GetTargetMap().IsNone())
		{
			ServerLoadAndWarp(Portal->GetTargetMap(), Portal->GetTargetTransform(), false);
		}
		break;
	case EPortalType::RaidField:
		if (!PS || !PS->IsInParty())
		{
			FChatMessage ErrorMsg;
			ErrorMsg.Sender = TEXT("시스템");
			ErrorMsg.Message = TEXT("파티에 속해있지 않습니다.");
			ErrorMsg.Channel = EChatChannel::Global;
			ServerSystemChat(ErrorMsg);
			return;
		}
		else if (!PS->IsPartyLeader())
		{
			FChatMessage ErrorMsg;
			ErrorMsg.Sender = TEXT("시스템");
			ErrorMsg.Message = TEXT("파티장이 아닙니다.");
			ErrorMsg.Channel = EChatChannel::Global;
			ServerSystemChat(ErrorMsg);
			return;
		}
		FName RequiredMap = Portal->GetRequiredMap();
		if (!AreAllPartyMembersInVillage(RequiredMap))
		{
			FChatMessage ErrorMsg;
			ErrorMsg.Sender = TEXT("시스템");
			ErrorMsg.Message = FString::Printf(TEXT("모두가 %s에 있지 않습니다"), *RequiredMap.ToString());
			ErrorMsg.Channel = EChatChannel::Global;
			ServerSystemChat(ErrorMsg);
			return;
		}
		ServerShowRaidConfirm(Portal->GetTargetMap(), Portal->GetTargetTransform(), RequiredMap);
		break;
	}
}

bool AUserPlayerController::FindRespawnMap(ATheDSPlayerState* PS, FName& FindVillage, FTransform& FindSpawn) const
{
	if (!PS) return false;

	if (PS->GetLastVillage() != NAME_None)
	{
		FindVillage = PS->GetLastVillage();
		FindSpawn = PS->GetLastVillageSpawn();
		return true;
	}
	if (RespawnData && PS->GetLastField() != NAME_None)
	{
		if (RespawnData->FindVillageByField(PS->GetLastField(), FindVillage, FindSpawn))
		{
			return true;
		}
	}
	FindVillage = FName("Village_2");
	FindSpawn = FTransform(FRotator::ZeroRotator, FVector(-1350.f, 3170.f, 200.f));
	return true;
}

bool AUserPlayerController::AreAllPartyMembersInVillage(FName RequiredVillage) const
{
	const ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (!PS) return false;
	const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
	bool bInVillage = true;
	for (const FPartyMember& M : Members)
	{
		if (const ATheDSPlayerState* PS_Member = Cast<ATheDSPlayerState>(M.Member))
		{
			if (PS_Member->GetLastVillage() != RequiredVillage)
			{
				bInVillage = false;
			}
		}
		else
		{
			bInVillage = false;
		}
	}
	return bInVillage;
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

void AUserPlayerController::ClientShowPartyInvite_Implementation(APlayerState* FromLeader)
{
	if (!InvitePartyWidgetClass) return;
	UInvitePartyWidget* InviteWidget = CreateWidget<UInvitePartyWidget>(this, InvitePartyWidgetClass);
	if (InviteWidget)
	{
		InviteWidget->Init(FromLeader);
		InviteWidget->AddToViewport();
	}
}

void AUserPlayerController::ServerRespondToInvite_Implementation(bool bAccepted, APlayerState* FromLeader)
{
	if (!FromLeader || !PlayerState) return;
	if (bAccepted)
	{
		if (APartyState* PartyState = GetWorld()->GetGameState<APartyState>())
		{
			PartyState->AcceptInvite(PlayerState, FromLeader); // 리더 기준으로 추가
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s declined the party invite from %s"),
			*PlayerState->GetPlayerName(),
			*FromLeader->GetPlayerName());
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

void AUserPlayerController::ServerSendChat_Implementation(const FString& Message, EChatChannel Channel)
{
	FChatMessage Chat;
	Chat.Message = Message;
	Chat.Channel = Channel;
	Chat.Sender = GetPlayerState<ATheDSPlayerState>()->GetNickname();

	if (Channel == EChatChannel::Global)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AUserPlayerController* PC = Cast<AUserPlayerController>(*It))
			{
				PC->ClientReceiveChat(Chat);
			}
		}
	}
	else if (Channel == EChatChannel::Party)
	{
		// 파티 멤버들만 전달
		ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
		if (PS)
		{
			const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
			for (const FPartyMember& Member : Members)
			{
				if (AUserPlayerController* PC = Cast<AUserPlayerController>(Member.Member->GetOwner()))
				{
					PC->ClientReceiveChat(Chat);
				}
			}
		}
	}
}

void AUserPlayerController::ClientReceiveChat_Implementation(const FChatMessage& Chat)
{
	if (ChattingWidgetInstance)
	{
		ChattingWidgetInstance->AddChat(Chat);
	}
}

void AUserPlayerController::ServerSystemChat_Implementation(const FChatMessage& Chat)
{
	ClientReceiveChat(Chat);
}

void AUserPlayerController::ServerSetLastVillage_Implementation(FName Village, FTransform Spawn)
{
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (PS) PS->SetLastVillage(Village, Spawn);
}

void AUserPlayerController::ServerSetLastField_Implementation(FName Field)
{
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (PS) PS->SetLastField(Field);
}

void AUserPlayerController::ServerLoadAndWarp_Implementation(FName LevelName, FTransform Spawn, bool bIsRaid)
{
	if (LevelName.IsNone()) return;

	FLatentActionInfo Latent; 
	Latent.CallbackTarget = this;
	UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, Latent);

	GetWorldTimerManager().SetTimer(PortalWarpTimer, [this, LevelName, Spawn, bIsRaid]()
		{
			if (ULevelStreaming* S = UGameplayStatics::GetStreamingLevel(this, LevelName))
			{
				if (S->IsLevelLoaded() && S->IsLevelVisible())
				{
					auto TeleportOne = [&](AUserPlayerController* PC)
						{
							if (!PC) return;
							if (APawn* P = PC->GetPawn())
							{
								if (ACharacter* C = Cast<ACharacter>(P))
								{
									// 텔레포트 안정화
									C->GetCharacterMovement()->StopMovementImmediately();
									C->TeleportTo(Spawn.GetLocation(), Spawn.GetRotation().Rotator());
									C->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
									C->ForceNetUpdate();
								}
								else
								{
									P->TeleportTo(Spawn.GetLocation(), Spawn.GetRotation().Rotator());
									P->ForceNetUpdate();
								}
							}
						};
					if (bIsRaid)
					{
						if (ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>())
						{
							const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
							for (const FPartyMember& M : Members)
							{
								if (AUserPlayerController* PC = Cast<AUserPlayerController>(M.Member->GetOwner()))
								{
									TeleportOne(PC);
									PC->ClientSetVisibleLevel(LevelName);
								}
							}
						}
					}
					else
					{
						TeleportOne(this);
						ClientSetVisibleLevel(LevelName);
					}
					GetWorldTimerManager().ClearTimer(PortalWarpTimer);
				}
			}
		}, 0.05f, true);
}

void AUserPlayerController::ClientSetVisibleLevel_Implementation(FName NewLevelName)
{
	if (CurrentStreamLevel != NAME_None && CurrentStreamLevel != NewLevelName)
	{
		UGameplayStatics::UnloadStreamLevel(this, CurrentStreamLevel, FLatentActionInfo(), true);
	}
	if (NewLevelName != NAME_None)
	{
		UGameplayStatics::LoadStreamLevel(this, NewLevelName, true, true, FLatentActionInfo());
	}

	CurrentStreamLevel = NewLevelName;
}

void AUserPlayerController::ServerShowRaidConfirm_Implementation(FName LevelName, FTransform Spawn, FName RequiredVillage)
{
	ClientShowRaidConfirm(LevelName, Spawn, RequiredVillage);
}

void AUserPlayerController::ClientShowRaidConfirm_Implementation(FName LevelName, FTransform Spawn, FName RequiredVillage)
{
	if (!RaidConfirmWidgetClass) return;
	URaidConfirmWidget* RaidConfirmWidget = CreateWidget<URaidConfirmWidget>(this, RaidConfirmWidgetClass);
	if (RaidConfirmWidget)
	{
		RaidConfirmWidget->Init(LevelName, Spawn, RequiredVillage);
		RaidConfirmWidget->AddToViewport();
	}
}

void AUserPlayerController::ServerRaidConfirmResult_Implementation(bool bAccept, FName LevelName, FTransform Spawn, FName RequiredVillage)
{
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (!PS || !PS->IsPartyLeader()) return;
	UE_LOG(LogTemp, Warning, TEXT("Raid Confirm Get"));
	if (!bAccept)
	{
		FChatMessage ErrorMsg;
		ErrorMsg.Sender = TEXT("시스템");
		ErrorMsg.Message = TEXT("레이드가 취소되었습니다");
		ErrorMsg.Channel = EChatChannel::Party;
		ServerSystemChat(ErrorMsg);
		return;
	}
	if (!AreAllPartyMembersInVillage(RequiredVillage))
	{
		FChatMessage ErrorMsg;
		ErrorMsg.Sender = TEXT("시스템");
		ErrorMsg.Message = FString::Printf(TEXT("모두가 %s에 있지 않습니다"), *RequiredVillage.ToString());
		ErrorMsg.Channel = EChatChannel::Party;
		ServerSystemChat(ErrorMsg);
		return;
	}
	// 서버에서 파티 멤버에게 적용
	ServerLoadAndWarp(LevelName, Spawn, true);
	//ServerStartRaidInstance(LevelName, RequiredVillage);
}

void AUserPlayerController::ServerStartRaidInstance_Implementation(FName BossMap, FName RequiredVillage)
{
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (!PS || !PS->IsPartyLeader() || !AreAllPartyMembersInVillage(RequiredVillage)) return;
	const FString PartyId = FString::Printf(TEXT("PartyLeader_%s"), *PS->GetPlayerName());
	const FString BossMapPath = FString::Printf(TEXT("/Game/ThsDS_Map/%s"), *BossMap.ToString());
	if (ADSGameMode* GM = GetWorld()->GetAuthGameMode<ADSGameMode>())
	{
		GM->StartBossInstance(PartyId, BossMapPath, [this](const FString& URL){
			if (URL.IsEmpty()) 
			{
				FChatMessage ErrorMsg;
				ErrorMsg.Sender = TEXT("시스템");
				ErrorMsg.Message = FString::Printf(TEXT("레이드 생성 실패"));
				ErrorMsg.Channel = EChatChannel::Party;
				ServerSystemChat(ErrorMsg);
				return;
			}
			PendingTravelURL = URL;
			PreparedCount = 0;
			const TArray<FPartyMember>& Ms = GetPlayerState<ATheDSPlayerState>()->GetReplicatedPartyMembers();
			ExpectedCount = Ms.Num() + 1; // +리더
			for (const FPartyMember& M : Ms)
				if (auto* PC = Cast<AUserPlayerController>(M.Member->GetOwner()))
					PC->ClientPrepareForInstanceTravel();
			ClientPrepareForInstanceTravel(); // 리더
			});
	}
}

void AUserPlayerController::ClientPrepareForInstanceTravel_Implementation()
{
	ServerNotifyPreparedForInstance();
}

void AUserPlayerController::ServerNotifyPreparedForInstance_Implementation()
{
	if (!HasAuthority()) return;
	PreparedCount++;
	if (PreparedCount >= ExpectedCount && !PendingTravelURL.IsEmpty())
	{
		ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>(); if (!PS) return;
		for (const FPartyMember& M : PS->GetReplicatedPartyMembers())
			if (auto* PC = Cast<AUserPlayerController>(M.Member->GetOwner()))
				PC->ClientTravelToBossInstance(PendingTravelURL);
		ClientTravelToBossInstance(PendingTravelURL);
		PreparedCount = 0;
		ExpectedCount = 0;
		PendingTravelURL.Reset();
	}
}

void AUserPlayerController::ClientTravelToBossInstance_Implementation(const FString& TravelURL)
{
	ClientTravel(TravelURL, TRAVEL_Absolute);
}

void AUserPlayerController::ClientShowEndingConfirm_Implementation(ADragonBoss* Dragon, bool bIsLeader)
{
	// WBP_EndingConfirm 위젯 생성/표시
	// - 리더: 확인/취소 버튼 → 확인 시 ServerConfirmEnding(Dragon, true) 호출
	// - 파티원: "파티장이 확인 중입니다" 문구만 표시(또는 OK만)
	if (!EndingConfirmWidgetClass || !Dragon) return;
	UEndingConfirmWidget* EndingConfirmWidget = CreateWidget<UEndingConfirmWidget>(this, EndingConfirmWidgetClass);
	if (EndingConfirmWidget)
	{
		EndingConfirmWidget->Init(Dragon, bIsLeader);
		EndingConfirmWidget->AddToViewport();
	}
}

void AUserPlayerController::ServerConfirmEnding_Implementation(ADragonBoss* Dragon, bool bAccept)
{
	if (!HasAuthority() || !Dragon) return;
	Dragon->bEndingInProgress = true;
	ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
	if (!PS || !PS->IsPartyLeader())
	{
		Dragon->bEndingInProgress = false;
		return;
	}
	if (!bAccept)
	{
		FChatMessage Msg;
		Msg.Sender = TEXT("시스템");
		Msg.Channel = EChatChannel::Party;
		Msg.Message = TEXT("엔딩이 취소되었습니다.");
		ServerSystemChat(Msg);
		Dragon->bEndingInProgress = false;
		return;
	}
	const TArray<FPartyMember>& Members = PS->GetReplicatedPartyMembers();
	const float EndDur = 8.f;
	for (const FPartyMember& M : Members)
		if (AUserPlayerController* MPC = Cast<AUserPlayerController>(M.Member->GetOwner()))
			MPC->ClientPlayEnding(EndDur);
	ClientPlayEnding(EndDur);
	FTimerHandle T;
	GetWorldTimerManager().SetTimer(T, [this, Members]()
		{
			for (const FPartyMember& M : Members)
				if (AUserPlayerController* MPC = Cast<AUserPlayerController>(M.Member->GetOwner()))
					MPC->ServerLoadAndWarp(FName("Village_2"), FTransform(FRotator::ZeroRotator, FVector(-1350, 3170, 200)), false);
			ServerLoadAndWarp(FName("Village_2"), FTransform(FRotator::ZeroRotator, FVector(-1350, 3170, 200)), false);
		}, EndDur, false);
}

void AUserPlayerController::ClientPlayEnding_Implementation(float Duration)
{
	if (!IsLocalController() || !EndingPlayWidgetClass) return;
	UEndingPlayWidget* EndingPlayWidget = CreateWidget<UEndingPlayWidget>(this, EndingPlayWidgetClass);
	if (EndingPlayWidget)
	{
		EndingPlayWidget->AddToViewport(2000);
		EndingPlayWidget->InitData(EndingBookData);
	}
}
