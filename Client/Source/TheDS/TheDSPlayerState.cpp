#include "TheDSPlayerState.h"
#include "GameFramework/Pawn.h"
#include "BaseStatComponent.h"
#include "BaseCharacter.h"
#include "UserPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "ServerGameInstance.h"
#include "InventoryComponent.h"

int32 ATheDSPlayerState::GetLevel() const
{
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(GetPawn()))
	{
		if (Player->Stat)
		{
			return Player->Stat->GetLevel();
		}
	}
	return 0;
}

void ATheDSPlayerState::SetPartyLeader(APlayerState* Leader)
{
	PartyLeader = Leader;
}

APlayerState* ATheDSPlayerState::GetPartyLeader() const
{
	return PartyLeader;
}

void ATheDSPlayerState::OnRep_PartyLeader()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (AUserPlayerController* UserPC = Cast<AUserPlayerController>(PC))
		{
			UserPC->ClientUpdatePartyUI();
		}
	}
}

void ATheDSPlayerState::OnRep_PartyMembers()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (AUserPlayerController* UserPC = Cast<AUserPlayerController>(PC))
		{
			UserPC->ClientUpdatePartyUI();
		}
	}
}

void ATheDSPlayerState::SetLastVillage(FName Village, const FTransform& Spawn)
{
	LastVisitedVillage = Village;
	LastVillageSpawn = Spawn;
}

void ATheDSPlayerState::SetLastField(FName Field)
{
	LastVisitedField = Field;
}

void ATheDSPlayerState::SetReplicatedPartyMembers(const TArray<FPartyMember>& Members)
{
	PartyMembers = Members;
	OnRep_PartyMembers();
}

void ATheDSPlayerState::SaveUserData()
{
    ABaseCharacter* Player = Cast<ABaseCharacter>(GetPawn());
    if (!Player) return;
    // 정보
    FString ID = UserID;
    FString Nick = Nickname;
    int32 Level = 1;
    float Exp = 0.0f;
    int32 Gold = 0;
    if (Player->Stat)
    {
        Level = Player->Stat->GetLevel();
        Exp = Player->Stat->GetCurrentEXP();
        Gold = Player->Stat->GetMoney();
    }
    TArray<FItemSaveData> SaveItems;
    if (Player->InventoryComponent)
    {
        auto CollectItems = [&](const TArray<FInventorySlot>& Slots)
            {
                for (const FInventorySlot& Slot : Slots)
                {
                    if (Slot.ItemData && Slot.Quantity > 0)
                    {
                        FItemSaveData NewData;
                        NewData.ItemID = Slot.ItemData->ItemID;
                        NewData.Amount = Slot.Quantity;
                        SaveItems.Add(NewData);
                    }
                }
            };
        CollectItems(Player->InventoryComponent->ConsumeSlots);
        CollectItems(Player->InventoryComponent->EquipmentSlots);
    }
    // 인벤토리->Json 파일로 변경
    FInventorySaveData InventorySave;
    InventorySave.Items = SaveItems;
    FString InventoryJsonString = "[]";
    if (SaveItems.Num() > 0)
    {
        FJsonObjectConverter::UStructToJsonObjectString(InventorySave, InventoryJsonString);
    }
    // 위치 정보
    FString MapName = GetWorld()->GetMapName();
    MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
    FVector Loc = Player->GetActorLocation();
    UServerGameInstance* GI = Cast<UServerGameInstance>(GetGameInstance());
    if (GI)
    {
        GI->RequestSaveGameData(ID, Nick, Level, Exp, Gold, MapName, Loc, InventoryJsonString);
    }
}

void ATheDSPlayerState::LoadUserData(const FString& DataString)
{
    TArray<FString> Tokens;
    DataString.ParseIntoArray(Tokens, TEXT(" "), true);
    if (Tokens.Num() < 7) return;
    // 공백 기준 데이터 분리
    int32 LoadedLevel = FCString::Atoi(*Tokens[0]);
    float LoadedExp = FCString::Atof(*Tokens[1]);
    int32 LoadedGold = FCString::Atoi(*Tokens[2]);
    FString LoadedMap = Tokens[3];
    float LoadedX = FCString::Atof(*Tokens[4]);
    float LoadedY = FCString::Atof(*Tokens[5]);
    float LoadedZ = FCString::Atof(*Tokens[6]);
    // 스탯 적용
    if (ABaseCharacter* Player = Cast<ABaseCharacter>(GetPawn()))
    {
        if (Player->Stat)
        {
            Player->Stat->SetLevel(LoadedLevel);
            Player->Stat->SetCurrentEXP(LoadedExp);
            Player->Stat->SetCurrentMoney(LoadedGold);
            Player->SetActorLocation(FVector(LoadedX, LoadedY, LoadedZ));
        }
    }

    // 인벤토리 JSON 파싱
    FString JsonString = "";
    for (int32 i = 7; i < Tokens.Num(); i++)
    {
        if (i > 7) JsonString += " ";
        JsonString += Tokens[i];
    }
    FInventorySaveData InventoryData;
    if (FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &InventoryData, 0, 0))
    {
        if (ABaseCharacter* Player = Cast<ABaseCharacter>(GetPawn()))
        {
            if (Player->InventoryComponent)
            {
                UServerGameInstance* GI = Cast<UServerGameInstance>(GetGameInstance());
                if (GI)
                {
                    for (const FItemSaveData& ItemData : InventoryData.Items)
                    {
                        UBaseItem* FoundItem = GI->GetItemByID(ItemData.ItemID);
                        if (FoundItem)
                        {
                            Player->InventoryComponent->AddItem(FoundItem, ItemData.Amount);
                        }
                    }
                }
            }
        }
    }
}

void ATheDSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATheDSPlayerState, PartyLeader);
	DOREPLIFETIME(ATheDSPlayerState, PartyMembers);
	DOREPLIFETIME(ATheDSPlayerState, Nickname);
	DOREPLIFETIME(ATheDSPlayerState, LastVisitedVillage);
	DOREPLIFETIME(ATheDSPlayerState, LastVillageSpawn);
	DOREPLIFETIME(ATheDSPlayerState, LastVisitedField);
    DOREPLIFETIME(ATheDSPlayerState, UserID);
}