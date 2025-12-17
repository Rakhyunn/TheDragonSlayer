#include "EndingPlayWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UEndingPlayWidget::NativeConstruct()
{
	Super::NativeConstruct();
    if (BTN_Prev) BTN_Prev->OnClicked.AddDynamic(this, &UEndingPlayWidget::OnPrev);
    if (BTN_Next) BTN_Next->OnClicked.AddDynamic(this, &UEndingPlayWidget::OnNext);
    RefreshPage();
    UpdateNav();
}

void UEndingPlayWidget::InitData(UEndingBookData* InData)
{
    Data = InData;
    Pages = Data ? Data->Pages : TArray<FEndingPage>{};
    PageIndex = 0;
    if (IsConstructed())
    {
        RefreshPage();
        UpdateNav();
    }
}

void UEndingPlayWidget::OnPrev()
{
    if (PageIndex > 0)
    {
        PageIndex--;
        RefreshPage();
        UpdateNav();
    }
}

void UEndingPlayWidget::OnNext()
{
    if (PageIndex + 1 < Pages.Num())
    {
        PageIndex++;
        RefreshPage();
        UpdateNav();
    }
}

void UEndingPlayWidget::RefreshPage()
{
    if (!Pages.IsValidIndex(PageIndex)) return;

    const FEndingPage& P = Pages[PageIndex];
    if (TXT_Title) TXT_Title->SetText(P.Title);
    if (TXT_Body)  TXT_Body->SetText(P.Body);
    if (TXT_PageIndicator)
    {
        const int32 Cur = PageIndex + 1;
        const int32 Total = Pages.Num();
        TXT_PageIndicator->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Cur, Total)));
    }
}

void UEndingPlayWidget::UpdateNav()
{
    const bool bHasPrev = (PageIndex > 0);
    const bool bHasNext = (PageIndex + 1 < Pages.Num());

    if (BTN_Prev) BTN_Prev->SetIsEnabled(bHasPrev);
    if (BTN_Next) BTN_Next->SetIsEnabled(bHasNext);
}
