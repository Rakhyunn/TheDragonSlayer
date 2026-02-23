#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerGameInstance.h"
#include "CharacterCreateWidget.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;
class UImage;
class UOverlay;
class UWidgetSwitcher;

// 캐릭터 생성 완료 델리게이트 → CharacterSelectWidget이 구독
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterCreated);

UCLASS()
class THEDS_API UCharacterCreateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCharacterCreated OnCharacterCreatedDelegate;

protected:
    // ── 페이지 전환: 0=직업 선택  1=닉네임 입력 ─────────────────────────
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* StepSwitcher;

    // ────────────────────────────────────────────────────────────────────
    //  Step 0 : 직업 선택
    // ────────────────────────────────────────────────────────────────────

    // 전사 카드: 이미지 + 이름 텍스트를 감싸는 버튼(Overlay 위에 투명 버튼 사용)
    UPROPERTY(meta = (BindWidget))
    UButton* Warrior_CardBtn;     // 전사 카드 전체를 덮는 투명 버튼

    UPROPERTY(meta = (BindWidget))
    UImage* Warrior_Image;       // 전사 직업 이미지

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Warrior_Label;       // "전사" 텍스트

    // 마법사 카드
    UPROPERTY(meta = (BindWidget))
    UButton* Mage_CardBtn;

    UPROPERTY(meta = (BindWidget))
    UImage* Mage_Image;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Mage_Label;

    UPROPERTY(meta = (BindWidget)) UButton* Step0_SelectBtn;
    UPROPERTY(meta = (BindWidget)) UButton* Step0_BackBtn;

    // ────────────────────────────────────────────────────────────────────
    //  Step 1 : 닉네임 입력
    // ────────────────────────────────────────────────────────────────────

    // 선택된 직업 다시 표시 (확인용)
    UPROPERTY(meta = (BindWidget)) UImage* Step1_Preview;

    // 선택한 직업 표시
    UPROPERTY(meta = (BindWidget)) UTextBlock* Step1_ClassText;

    UPROPERTY(meta = (BindWidget)) UEditableTextBox* NicknameInputBox;
    UPROPERTY(meta = (BindWidget)) UButton* CheckNicknameBtn;
    UPROPERTY(meta = (BindWidget)) UTextBlock* NicknameResultText;
    UPROPERTY(meta = (BindWidget)) UButton* CreateBtn;
    UPROPERTY(meta = (BindWidget)) UButton* Step1_BackBtn;
    UPROPERTY(meta = (BindWidget)) UTextBlock* ResultText;

    // ── 에디터 설정 ─────────────────────────────────────────────────────
    // Step 1에서 사용할 RenderTarget (선택한 직업의 캐릭터 프리뷰용)
    UPROPERTY(EditDefaultsOnly, Category = "Preview")
    class UTextureRenderTarget2D* PreviewRenderTarget;

    // 더미 액터 태그 (Step 1용 - Warrior / Mage 각 1개)
    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName WarriorPreviewTag = TEXT("CreateWarrior");

    UPROPERTY(EditDefaultsOnly, Category = "DummyActor")
    FName MagePreviewTag = TEXT("CreateMage");

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    FString SelectedClass = TEXT("Warrior"); // 현재 선택 직업
    bool    bNicknameChecked = false;           // 중복 확인 통과 여부
    FString LastCheckedNickname;                 // 마지막으로 확인한 닉네임

    // ── 직업 선택 콜백 ──────────────────────────────────────────────────
    UFUNCTION() void OnWarriorCardClicked();
    UFUNCTION() void OnMageCardClicked();

    // ── Step 전환 버튼 ──────────────────────────────────────────────────
    UFUNCTION() void OnStep0NextClicked();   // 직업 확정 → 닉네임 입력 화면
    UFUNCTION() void OnStep0BackClicked();   // 뒤로 (선택 화면 닫기)
    UFUNCTION() void OnStep1BackClicked();   // 뒤로 (직업 선택으로)

    // ── 닉네임 ──────────────────────────────────────────────────────────
    UFUNCTION() void OnNicknameTextChanged(const FText& Text);
    UFUNCTION() void OnCheckNicknameClicked();
    UFUNCTION() void OnCharacterCreateClicked();

    // 직업 선택 공통 처리 (하이라이트 토글 + 텍스트 갱신)
    void SelectClass(const FString& ClassName);
    AActor* FindDummyActor(FName Tag) const;
    void HideAllPreviewDummies();
    void ShowClassPreview(const FString& ClassName); // Step 1에서 직업별 3D 프리뷰 표시

    // 결과 텍스트 표시 (3초 자동 소거)
    void ShowResult(const FString& Msg);
    void ShowNicknameResult(const FString& Msg);
    FTimerHandle ResultTimerHandle;
    FTimerHandle NicknameResultTimerHandle;

    UServerGameInstance* GetGI() const;
};
