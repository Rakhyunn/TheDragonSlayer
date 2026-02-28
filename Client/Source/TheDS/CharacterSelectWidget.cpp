#include "CharacterSelectWidget.h"
#include "CharacterCreateWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

void UCharacterSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 입장 버튼
    if (Slot0_SelectBtn) Slot0_SelectBtn->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnSlot0SelectClicked);
    if (Slot1_SelectBtn) Slot1_SelectBtn->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnSlot1SelectClicked);
    if (Slot2_SelectBtn) Slot2_SelectBtn->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnSlot2SelectClicked);

    // 생성 버튼
    if (Slot0_CreateBtn) Slot0_CreateBtn->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnSlot0CreateClicked);
    if (Slot1_CreateBtn) Slot1_CreateBtn->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnSlot1CreateClicked);
    if (Slot2_CreateBtn) Slot2_CreateBtn->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnSlot2CreateClicked);

    if (StatusText) StatusText->SetVisibility(ESlateVisibility::Collapsed);

    // RenderTarget → Image 연결
    BindRenderTargets();

    // 더미 액터 모두 숨김 (초기 상태)
    for (int32 i = 0; i < 3; i++)
        HideAllDummiesForSlot(i);

    // 캐릭터 목록 요청
    RequestAndParseCharacterList();
}

void UCharacterSelectWidget::NativeDestruct()
{
    for (int32 i = 0; i < 3; i++)
        HideAllDummiesForSlot(i);
    Super::NativeDestruct();
}

void UCharacterSelectWidget::RequestAndParseCharacterList()
{
    UServerGameInstance* GI = GetGI();
    if (!GI) { ShowStatus(TEXT("GameInstance Error")); return; }

    FString Response = GI->GetCharacterList();
    ParseCharacterList(Response);
    RefreshAllSlots();
}

void UCharacterSelectWidget::ParseCharacterList(const FString& Response)
{
    SlotDataList.SetNum(3);
    for (auto& S : SlotDataList) S = FCharacterSlotData();

    // 응답 형식: "CharacterList <count> <nick> <level> <class> ..."
    TArray<FString> Tokens;
    Response.ParseIntoArrayWS(Tokens);
    if (Tokens.Num() < 2 || Tokens[0] != TEXT("CharacterList")) return;

    int32 Count = FMath::Clamp(FCString::Atoi(*Tokens[1]), 0, 3);
    for (int32 i = 0; i < Count; i++)
    {
        int32 Base = 2 + i * 3; // nick[Base] level[Base+1] class[Base+2]
        if (Base + 2 >= Tokens.Num()) break;
        SlotDataList[i].Nickname = Tokens[Base];
        SlotDataList[i].Level = FCString::Atoi(*Tokens[Base + 1]);
        SlotDataList[i].CharClass = Tokens[Base + 2];
    }
}

void UCharacterSelectWidget::RefreshAllSlots()
{
    for (int32 i = 0; i < 3; i++)
        RefreshSlot(i, SlotDataList[i]);
}

void UCharacterSelectWidget::RefreshSlot(int32 Index, const FCharacterSlotData& Data)
{
    // 슬롯별 위젯 포인터를 인덱스로 가져오는 헬퍼 람다
    auto Overlay = [&]() -> UOverlay* { return Index == 0 ? Slot0_Overlay : Index == 1 ? Slot1_Overlay : Slot2_Overlay;   };
    auto Preview = [&]() -> UImage* { return Index == 0 ? Slot0_Preview : Index == 1 ? Slot1_Preview : Slot2_Preview;   };
    auto NickTB = [&]() -> UTextBlock* { return Index == 0 ? Slot0_Nickname : Index == 1 ? Slot1_Nickname : Slot2_Nickname;  };
    auto LevelTB = [&]() -> UTextBlock* { return Index == 0 ? Slot0_Level : Index == 1 ? Slot1_Level : Slot2_Level;     };
    auto ClassTB = [&]() -> UTextBlock* { return Index == 0 ? Slot0_Class : Index == 1 ? Slot1_Class : Slot2_Class;     };
    auto SelectBtn = [&]() -> UButton* { return Index == 0 ? Slot0_SelectBtn : Index == 1 ? Slot1_SelectBtn : Slot2_SelectBtn; };
    auto CreateBtn = [&]() -> UButton* { return Index == 0 ? Slot0_CreateBtn : Index == 1 ? Slot1_CreateBtn : Slot2_CreateBtn; };

    bool bEmpty = Data.IsEmpty();

    UE_LOG(LogTemp, Warning, TEXT("RefreshSlot[%d]: Empty=%d, Class=%s"),
        Index, bEmpty, *Data.CharClass);

    // 캐릭터 있음: 정보 패널 표시 / 생성 버튼 숨김
    // 캐릭터 없음: 정보 패널 숨김 / 생성 버튼 표시
    if (Overlay())   Overlay()->SetVisibility(bEmpty ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    if (CreateBtn()) CreateBtn()->SetVisibility(bEmpty ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    if (!bEmpty)
    {
        if (Preview())  Preview()->SetVisibility(ESlateVisibility::Visible);
        if (NickTB())   NickTB()->SetText(FText::FromString(Data.Nickname));
        if (LevelTB())  LevelTB()->SetText(FText::FromString(FString::Printf(TEXT("Lv.%d"), Data.Level)));
        if (ClassTB())  ClassTB()->SetText(FText::FromString(Data.CharClass));
        if (SelectBtn()) SelectBtn()->SetIsEnabled(true);
    }

    // 더미 캐릭터 액터 표시 여부
    ShowDummyForSlot(Index, bEmpty ? TEXT("") : Data.CharClass);
}

void UCharacterSelectWidget::BindRenderTargets()
{
    auto Bind = [](UTextureRenderTarget2D* RT, UImage* Img)
        {
            if (!RT || !Img) return;
            FSlateBrush Brush;
            Brush.SetResourceObject(RT);
            Brush.ImageSize = FVector2D(256.f, 512.f);
            Img->SetBrush(Brush);
        };
    Bind(RenderTarget0, Slot0_Preview);
    Bind(RenderTarget1, Slot1_Preview);
    Bind(RenderTarget2, Slot2_Preview);
}

AActor* UCharacterSelectWidget::FindDummyActor(FName Tag) const
{
    if (!GetWorld()) return nullptr;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
        if (It->ActorHasTag(Tag)) return *It;
    return nullptr;
}

void UCharacterSelectWidget::HideAllDummiesForSlot(int32 SlotIndex)
{
    TArray<FName> Tags;
    if (SlotIndex == 0) Tags = { WarriorTag0, MageTag0, EmptyTag0 };
    if (SlotIndex == 1) Tags = { WarriorTag1, MageTag1, EmptyTag1 };
    if (SlotIndex == 2) Tags = { WarriorTag2, MageTag2, EmptyTag2 };

    for (FName Tag : Tags)
    {
        AActor* A = FindDummyActor(Tag);
        if (A) A->SetActorHiddenInGame(true);
    }
}

void UCharacterSelectWidget::ShowDummyForSlot(int32 SlotIndex, const FString& CharClass)
{
    // 먼저 해당 슬롯의 모든 더미 숨김
    HideAllDummiesForSlot(SlotIndex);

    // 직업에 따라 보여줄 더미 선택
    FName ShowTag = NAME_None;

    if (CharClass == TEXT("Warrior"))
    {
        ShowTag = SlotIndex == 0 ? WarriorTag0 : SlotIndex == 1 ? WarriorTag1 : WarriorTag2;
    }
    else if (CharClass == TEXT("Mage"))
    {
        ShowTag = SlotIndex == 0 ? MageTag0 : SlotIndex == 1 ? MageTag1 : MageTag2;
    }
    else // 빈 슬롯 (CharClass == "")
    {
        ShowTag = SlotIndex == 0 ? EmptyTag0 : SlotIndex == 1 ? EmptyTag1 : EmptyTag2;
    }

    AActor* A = FindDummyActor(ShowTag);
    if (A) A->SetActorHiddenInGame(false);
}

void UCharacterSelectWidget::ShowStatus(const FString& Msg)
{
    if (!StatusText) return;
    GetWorld()->GetTimerManager().ClearTimer(StatusTimerHandle);
    StatusText->SetText(FText::FromString(Msg));
    FTimerDelegate Del;
    Del.BindLambda([this]() { if (StatusText) StatusText->SetText(FText::GetEmpty()); });
    GetWorld()->GetTimerManager().SetTimer(StatusTimerHandle, Del, 3.f, false);
}

UServerGameInstance* UCharacterSelectWidget::GetGI() const
{
    return GetWorld() ? GetWorld()->GetGameInstance<UServerGameInstance>() : nullptr;
}

void UCharacterSelectWidget::OnSlot0SelectClicked() { HandleSelectSlot(0); }
void UCharacterSelectWidget::OnSlot1SelectClicked() { HandleSelectSlot(1); }
void UCharacterSelectWidget::OnSlot2SelectClicked() { HandleSelectSlot(2); }
void UCharacterSelectWidget::OnSlot0CreateClicked() { HandleCreateSlot(0); }
void UCharacterSelectWidget::OnSlot1CreateClicked() { HandleCreateSlot(1); }
void UCharacterSelectWidget::OnSlot2CreateClicked() { HandleCreateSlot(2); }

void UCharacterSelectWidget::HandleSelectSlot(int32 Index)
{
    if (!SlotDataList.IsValidIndex(Index)) return;
    const FCharacterSlotData& Data = SlotDataList[Index];
    if (Data.IsEmpty()) return;

    UServerGameInstance* GI = GetGI();
    if (!GI) return;

    ShowStatus(FString::Printf(TEXT("%s 로 입장 중..."), *Data.Nickname));

    // SELECTCHARACTER 전송 → 서버 세션에 CurrentCharacter_ 세팅
    FString Response = GI->SelectCharacter(Data.Nickname);

    if (Response.StartsWith(TEXT("LoadData")))
    {
        // 더미 액터 모두 숨기고 게임 서버로 이동
        for (int32 i = 0; i < 3; i++)
            HideAllDummiesForSlot(i);
        RemoveFromParent();

        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            FInputModeGameOnly GameInput;
            PC->SetInputMode(GameInput);
            PC->bShowMouseCursor = false;
            //FString Options = FString::Printf(TEXT("?UserID=%s?Nickname=%s"), *GI->GetLoggedInID(), *Data.Nickname);
            //FString ServerURL = TEXT("127.0.0.1:7777") + Options;
            //PC->ClientTravel(ServerURL, TRAVEL_Absolute);
            // [방법 B] 에디터 테스트 / 싱글 플레이 방식으로 열 때 (기존 방식)
             //FName NextLevel = TEXT("Basic");
             //UGameplayStatics::OpenLevel(this, NextLevel, true, FString::Printf(TEXT("UserID=%s?Nickname=%s")));
#if WITH_EDITOR
             // 에디터 PIE 테스트용
             FString Options = FString::Printf(TEXT("UserID=%s?Nickname=%s"), *GI->GetLoggedInID(), *Data.Nickname);
             UGameplayStatics::OpenLevel(GetWorld(), TEXT("Basic"), true, Options);
#else
             // 빌드(데디케이티드 서버) 테스트용
             FString URL = FString::Printf(TEXT("127.0.0.1:7777?UserID=%s?Nickname=%s"), *GI->GetLoggedInID(), *Data.Nickname);
             PC->ClientTravel(URL, TRAVEL_Absolute);
#endif
        }
    }
    else
    {
        ShowStatus(FString::Printf(TEXT("입장 실패: %s"), *Response));
    }
}

void UCharacterSelectWidget::HandleCreateSlot(int32 Index)
{
    if (!CharacterCreateWidgetClass) return;
    SetVisibility(ESlateVisibility::Hidden);
    //RemoveFromParent();
    UCharacterCreateWidget* CharacterCreateWidgetInstance = CreateWidget<UCharacterCreateWidget>(GetWorld(), CharacterCreateWidgetClass);
    if (CharacterCreateWidgetInstance)
    {
        CharacterCreateWidgetInstance->OnCharacterCreatedDelegate.AddDynamic(this, &UCharacterSelectWidget::OnCharacterCreated);
        CharacterCreateWidgetInstance->AddToViewport(10);
    }
}

void UCharacterSelectWidget::OnCharacterCreated()
{
    SetVisibility(ESlateVisibility::Visible);
    RequestAndParseCharacterList();
}
