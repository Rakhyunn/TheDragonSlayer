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
		ShopWidgetInstance->InitShop(Merchant); // 아이템 설정 등
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
		if (ChattingWidgetInstance)
		{
			FChatMessage ErrorMsg;
			ErrorMsg.Sender = TEXT("시스템");
			ErrorMsg.Message = TEXT("파티에 속해있지 않습니다.");
			ErrorMsg.Channel = EChatChannel::Global;

			ChattingWidgetInstance->AddChat(ErrorMsg);
		}
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

	switch (Portal->GetPortalType())
	{
	case EPortalType::Village:
	case EPortalType::MonsterField:
		if (!Portal->GetTargetMap().IsNone())
		{
			ServerLoadAndWarp(Portal->GetTargetMap(), Portal->GetTargetTransform(), false);
		}
		break;
	case EPortalType::RaidField:
		ATheDSPlayerState* PS = GetPlayerState<ATheDSPlayerState>();
		if (!PS || !PS->IsInParty())
		{
			if (ChattingWidgetInstance)
			{
				FChatMessage ErrorMsg;
				ErrorMsg.Sender = TEXT("시스템");
				ErrorMsg.Message = TEXT("파티에 속해있지 않습니다.");
				ErrorMsg.Channel = EChatChannel::Global;

				ChattingWidgetInstance->AddChat(ErrorMsg);
			}
			return;
		}
		else if (!PS->IsPartyLeader())
		{
			if (ChattingWidgetInstance)
			{
				FChatMessage ErrorMsg;
				ErrorMsg.Sender = TEXT("시스템");
				ErrorMsg.Message = TEXT("파티장이 아닙니다.");
				ErrorMsg.Channel = EChatChannel::Global;

				ChattingWidgetInstance->AddChat(ErrorMsg);
			}
			return;
		}
		ClientShowRaidConfirm(Portal->GetTargetMap(), Portal->GetTargetTransform());
		break;
	}
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
		InviteWidget->Init(FromLeader); // FromLeader 정보 UI에 표시
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

void AUserPlayerController::ServerLoadAndWarp_Implementation(FName LevelName, FTransform Spawn, bool bIsRaid)
{
	if (LevelName.IsNone()) return;

	// 서버도 해당 서브레벨 로드/가시화
	FLatentActionInfo Latent; 
	Latent.CallbackTarget = this;
	UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, Latent);

	// 서버에서 레벨 로딩 완료 대기 후, (파티면) 전원 텔레포트 / (개별이면) 자신만 텔레포트
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
									C->ForceNetUpdate(); // 즉시 복제
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
								}
							}
						}
					}
					else
					{
						TeleportOne(this);
					}
					// 모든 클라에게도 해당 서브레벨 로드/가시화 지시 (클라는 텔레포트 하지 않음!)
					MulticastLoadAndWarp(LevelName);
					GetWorldTimerManager().ClearTimer(PortalWarpTimer);
				}
			}
		}, 0.05f, true);
}

void AUserPlayerController::MulticastLoadAndWarp_Implementation(FName LevelName)
{
	FLatentActionInfo Latent; 
	Latent.CallbackTarget = this;
	UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, Latent);
}

void AUserPlayerController::ClientShowRaidConfirm_Implementation(FName LevelName, FTransform Spawn)
{
	// "레이드 입장?" Yes/No UI 출력
	// Yes일 떄
	ServerRaidConfirmResult(/*bAccept*/true, LevelName, Spawn);
	// No일 때
	// Server_RaidConfirmResult(false, LevelName, Spawn);
}

void AUserPlayerController::ServerRaidConfirmResult_Implementation(bool bAccept, FName LevelName, FTransform Spawn)
{
	if (!bAccept) return;
	// 서버에서 파티 멤버 모두에게 적용
	ServerLoadAndWarp(LevelName, Spawn, true);
}
