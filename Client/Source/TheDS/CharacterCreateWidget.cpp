#include "CharacterCreateWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"

void UCharacterCreateWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Step 0 버튼
    if (Warrior_CardBtn)      Warrior_CardBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnWarriorCardClicked);
    if (Mage_CardBtn)         Mage_CardBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnMageCardClicked);
    if (Step0_SelectBtn)  Step0_SelectBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnStep0NextClicked);
    if (Step0_BackBtn)    Step0_BackBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnStep0BackClicked);

    // Step 1 버튼
    if (Step1_BackBtn)    Step1_BackBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnStep1BackClicked);
    if (NicknameInputBox) NicknameInputBox->OnTextChanged.AddDynamic(this, &UCharacterCreateWidget::OnNicknameTextChanged);
    if (CheckNicknameBtn) CheckNicknameBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnCheckNicknameClicked);
    if (CreateBtn)        CreateBtn->OnClicked.AddDynamic(this, &UCharacterCreateWidget::OnCharacterCreateClicked);

    // Step 0으로 시작
    if (StepSwitcher) StepSwitcher->SetActiveWidgetIndex(0);
    SelectClass(TEXT("Warrior"));

    // Step 1 프리뷰 RenderTarget 바인딩
    if (PreviewRenderTarget && Step1_Preview)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(PreviewRenderTarget);
        Brush.ImageSize = FVector2D(400.f, 800.f);
        Step1_Preview->SetBrush(Brush);
    }

    // 초기: Step 1 더미 숨김
    HideAllPreviewDummies();

    if (ResultText)         ResultText->SetText(FText::GetEmpty());
    if (NicknameResultText) NicknameResultText->SetText(FText::GetEmpty());
}

void UCharacterCreateWidget::NativeDestruct()
{
    HideAllPreviewDummies();
    Super::NativeDestruct();
}

void UCharacterCreateWidget::OnWarriorCardClicked() { SelectClass(TEXT("Warrior")); }
void UCharacterCreateWidget::OnMageCardClicked() { SelectClass(TEXT("Mage")); }

void UCharacterCreateWidget::OnStep0NextClicked()
{
    if (Step1_ClassText)
        Step1_ClassText->SetText(FText::FromString(
            FString::Printf(TEXT("직업: %s"), *SelectedClass)));

    if (StepSwitcher) StepSwitcher->SetActiveWidgetIndex(1);
    
    ShowClassPreview(SelectedClass);
    
    // 닉네임 입력 초기화
    if (NicknameInputBox) NicknameInputBox->SetText(FText::GetEmpty());
    bNicknameChecked = false;
    LastCheckedNickname.Empty();
    if (NicknameResultText) NicknameResultText->SetText(FText::GetEmpty());
    if (ResultText)         ResultText->SetText(FText::GetEmpty());
}

void UCharacterCreateWidget::OnStep0BackClicked()
{
    OnCharacterCreatedDelegate.Broadcast();
    RemoveFromParent();
}

void UCharacterCreateWidget::OnStep1BackClicked()
{
    if (StepSwitcher) StepSwitcher->SetActiveWidgetIndex(0);
    HideAllPreviewDummies();
}

void UCharacterCreateWidget::OnNicknameTextChanged(const FText& Text)
{
    if (Text.ToString() != LastCheckedNickname)
    {
        bNicknameChecked = false;
        if (NicknameResultText) NicknameResultText->SetText(FText::GetEmpty());
    }
}

void UCharacterCreateWidget::OnCheckNicknameClicked()
{
    if (!NicknameInputBox) return;

    FString Nickname = NicknameInputBox->GetText().ToString().TrimStartAndEnd();
    if (Nickname.IsEmpty())
    {
        ShowNicknameResult(TEXT("닉네임을 입력하세요"));
        return;
    }

    UServerGameInstance* GI = GetGI();
    if (!GI) { ShowNicknameResult(TEXT("서버 연결 오류")); return; }

    FString Result = GI->CheckNickname(Nickname);

    if (Result == TEXT("Nickname Use Possible"))
    {
        bNicknameChecked = true;
        LastCheckedNickname = Nickname;
        ShowNicknameResult(TEXT("✓ 사용 가능한 닉네임입니다"));
    }
    else if (Result == TEXT("Nickname Use Impossible"))
    {
        bNicknameChecked = false;
        ShowNicknameResult(TEXT("✗ 이미 사용 중인 닉네임입니다"));
    }
    else
    {
        bNicknameChecked = false;
        ShowNicknameResult(FString::Printf(TEXT("확인 실패: %s"), *Result));
    }
}

void UCharacterCreateWidget::OnCharacterCreateClicked()
{
    if (!NicknameInputBox) return;

    FString Nickname = NicknameInputBox->GetText().ToString().TrimStartAndEnd();

    if (Nickname.IsEmpty())
    {
        ShowResult(TEXT("닉네임을 입력하세요"));
        return;
    }

    // 중복 확인 미실시 또는 입력 후 텍스트 변경
    if (!bNicknameChecked || Nickname != LastCheckedNickname)
    {
        ShowResult(TEXT("닉네임 중복 확인을 먼저 해주세요"));
        return;
    }

    UServerGameInstance* GI = GetGI();
    if (!GI) { ShowResult(TEXT("서버 연결 오류")); return; }

    // CREATECHARACTER 요청
    FString Result = GI->CreateCharacter(Nickname, SelectedClass);

    // 응답: "Character Create Success <nickname>"
    if (Result.StartsWith(TEXT("Character Create Success")))
    {
        ShowResult(FString::Printf(TEXT("%s (%s) 생성 완료!"), *Nickname, *SelectedClass));

        // 선택 화면에 알림 브로드캐스트
        OnCharacterCreatedDelegate.Broadcast();

        // 0.8초 후 자동 닫기
        FTimerHandle CloseTimer;
        GetWorld()->GetTimerManager().SetTimer(CloseTimer,
            FTimerDelegate::CreateLambda([this]() { RemoveFromParent(); }),
            0.8f, false);
    }
    else if (Result.StartsWith(TEXT("Create Failed: Nickname")))
    {
        // 혹시 서버 측에서 중복 발견 (경쟁 조건 등)
        bNicknameChecked = false;
        ShowResult(TEXT("닉네임 중복! 다시 확인해주세요"));
    }
    else if (Result == TEXT("Create Failed"))
    {
        ShowResult(TEXT("생성 실패 (최대 3개 초과 또는 서버 오류)"));
    }
    else
    {
        ShowResult(FString::Printf(TEXT("생성 실패: %s"), *Result));
    }
}

void UCharacterCreateWidget::SelectClass(const FString& ClassName)
{
    SelectedClass = ClassName;
    bool bWarrior = (ClassName == TEXT("Warrior"));
    bool bMage = (ClassName == TEXT("Mage"));

    // 하이라이트: 선택된 카드만 밝게, 나머지 어둡게
    FLinearColor Active = FLinearColor(1.f, 1.f, 1.f, 1.f);
    FLinearColor Inactive = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);

    if (Warrior_Image) Warrior_Image->SetColorAndOpacity(bWarrior ? Active : Inactive);
    if (Mage_Image)    Mage_Image->SetColorAndOpacity(bMage ? Active : Inactive);

    // 라벨 색상
    FSlateColor ActiveLabel = FSlateColor(FLinearColor(1.f, 0.9f, 0.f, 1.f));
    FSlateColor InactiveLabel = FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.f));

    if (Warrior_Label) Warrior_Label->SetColorAndOpacity(bWarrior ? ActiveLabel : InactiveLabel);
    if (Mage_Label)    Mage_Label->SetColorAndOpacity(bMage ? ActiveLabel : InactiveLabel);
}

AActor* UCharacterCreateWidget::FindDummyActor(FName Tag) const
{
    if (!GetWorld()) return nullptr;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
        if (It->ActorHasTag(Tag)) return *It;
    return nullptr;
}

void UCharacterCreateWidget::HideAllPreviewDummies()
{
    AActor* W = FindDummyActor(WarriorPreviewTag);
    AActor* M = FindDummyActor(MagePreviewTag);
    if (W) W->SetActorHiddenInGame(true);
    if (M) M->SetActorHiddenInGame(true);
}

void UCharacterCreateWidget::ShowClassPreview(const FString& ClassName)
{
    HideAllPreviewDummies();
    FName ShowTag = (ClassName == TEXT("Warrior")) ? WarriorPreviewTag : MagePreviewTag;
    AActor* A = FindDummyActor(ShowTag);
    if (A) A->SetActorHiddenInGame(false);
}

void UCharacterCreateWidget::ShowResult(const FString& Msg)
{
    GetWorld()->GetTimerManager().ClearTimer(ResultTimerHandle);
    if (ResultText) ResultText->SetText(FText::FromString(Msg));
    FTimerDelegate Del;
    Del.BindLambda([this]() { if (ResultText) ResultText->SetText(FText::GetEmpty()); });
    GetWorld()->GetTimerManager().SetTimer(ResultTimerHandle, Del, 3.f, false);
}

void UCharacterCreateWidget::ShowNicknameResult(const FString& Msg)
{
    GetWorld()->GetTimerManager().ClearTimer(NicknameResultTimerHandle);
    if (NicknameResultText) NicknameResultText->SetText(FText::FromString(Msg));
    FTimerDelegate Del;
    Del.BindLambda([this]() { if (NicknameResultText) NicknameResultText->SetText(FText::GetEmpty()); });
    GetWorld()->GetTimerManager().SetTimer(NicknameResultTimerHandle, Del, 3.f, false);
}

UServerGameInstance* UCharacterCreateWidget::GetGI() const
{
    return GetWorld() ? GetWorld()->GetGameInstance<UServerGameInstance>() : nullptr;
}
