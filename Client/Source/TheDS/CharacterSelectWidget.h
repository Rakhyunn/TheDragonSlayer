#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerGameInstance.h"
#include "CharacterSelectWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UOverlay;
class UCharacterCreateWidget;

USTRUCT(BlueprintType)
struct FCharacterSlotData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString Nickname = TEXT("");
	UPROPERTY(BlueprintReadOnly) int32   Level = 0;
	UPROPERTY(BlueprintReadOnly) FString CharClass = TEXT("");

	bool IsEmpty() const { return Nickname.IsEmpty(); }
};

UCLASS()
class THEDS_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    // 캐릭터가 있을 때 보이는 패널 전체
    UPROPERTY(meta = (BindWidget)) UOverlay* Slot0_Overlay;
    // SceneCapture RenderTarget 이미지
    UPROPERTY(meta = (BindWidget)) UImage* Slot0_Preview;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot0_Nickname;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot0_Level;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot0_Class;
    UPROPERTY(meta = (BindWidget)) UButton* Slot0_SelectBtn;
    UPROPERTY(meta = (BindWidget)) UButton* Slot0_CreateBtn;

    // ── BindWidget : 슬롯 1 ─────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) UOverlay* Slot1_Overlay;
    UPROPERTY(meta = (BindWidget)) UImage* Slot1_Preview;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot1_Nickname;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot1_Level;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot1_Class;
    UPROPERTY(meta = (BindWidget)) UButton* Slot1_SelectBtn;
    UPROPERTY(meta = (BindWidget)) UButton* Slot1_CreateBtn;

    // ── BindWidget : 슬롯 2 ─────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) UOverlay* Slot2_Overlay;
    UPROPERTY(meta = (BindWidget)) UImage* Slot2_Preview;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot2_Nickname;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot2_Level;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Slot2_Class;
    UPROPERTY(meta = (BindWidget)) UButton* Slot2_SelectBtn;
    UPROPERTY(meta = (BindWidget)) UButton* Slot2_CreateBtn;

    // ── BindWidget : 공통 ───────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) UTextBlock* StatusText;

    // ── 에디터 설정 ─────────────────────────────────────────────────────
    // 슬롯별 RenderTarget (SceneCapture2D 가 찍는 대상)
    UPROPERTY(EditDefaultsOnly, Category = "Preview|RenderTarget")
    class UTextureRenderTarget2D* RenderTarget0;

    UPROPERTY(EditDefaultsOnly, Category = "Preview|RenderTarget")
    class UTextureRenderTarget2D* RenderTarget1;

    UPROPERTY(EditDefaultsOnly, Category = "Preview|RenderTarget")
    class UTextureRenderTarget2D* RenderTarget2;

    // 레벨에 배치된 더미 액터 태그
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName WarriorTag0 = TEXT("Warrior_0");
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName MageTag0 = TEXT("Mage_0");
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName EmptyTag0 = TEXT("Empty_0");

    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName WarriorTag1 = TEXT("Warrior_1");
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName MageTag1 = TEXT("Mage_1");
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName EmptyTag1 = TEXT("Empty_1");

    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName WarriorTag2 = TEXT("Warrior_2");
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName MageTag2 = TEXT("Mage_2");
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName EmptyTag2 = TEXT("Empty_2");

    // 캐릭터 생성 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UCharacterCreateWidget> CharacterCreateWidgetClass;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct()  override;

private:
    TArray<FCharacterSlotData> SlotDataList; // 파싱된 슬롯 데이터 (항상 크기 3)

    // 캐릭터 목록 요청·파싱·갱신
    void RequestAndParseCharacterList();
    void ParseCharacterList(const FString& Response);
    void RefreshAllSlots();
    void RefreshSlot(int32 Index, const FCharacterSlotData& Data);

    // RenderTarget → Image 브러시 바인딩
    void BindRenderTargets();

    // 슬롯별 더미 액터 표시: CharClass가 "Warrior"면 Warrior_N, "Mage"면 Mage_N, ""면 Empty_N
    AActor* FindDummyActor(FName Tag) const;
    void HideAllDummiesForSlot(int32 SlotIndex);
    void ShowDummyForSlot(int32 SlotIndex, const FString& CharClass);

    // 상태 텍스트 (3초 자동 소거)
    void ShowStatus(const FString& Msg);
    FTimerHandle StatusTimerHandle;

    UServerGameInstance* GetGI() const;

    // ── 버튼 콜백 ───────────────────────────────────────────────────────
    UFUNCTION() void OnSlot0SelectClicked();
    UFUNCTION() void OnSlot1SelectClicked();
    UFUNCTION() void OnSlot2SelectClicked();
    UFUNCTION() void OnSlot0CreateClicked();
    UFUNCTION() void OnSlot1CreateClicked();
    UFUNCTION() void OnSlot2CreateClicked();

    void HandleSelectSlot(int32 Index);
    void HandleCreateSlot(int32 Index);

    // 캐릭터 생성 완료 콜백 → 목록 갱신
    UFUNCTION() void OnCharacterCreated();
};
