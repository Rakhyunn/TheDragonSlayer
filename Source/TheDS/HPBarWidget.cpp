#include "HPBarWidget.h"
#include "Components/ProgressBar.h"
#include "BaseStatComponent.h"

void UHPBarWidget::NativeConstruct()
{
	if (PB_HPBar)
	{
		PB_HPBar->SetPercent(1.0f); // �ʱⰪ
	}
}

void UHPBarWidget::BindHp(UBaseStatComponent* stat)
{
	if (!IsValid(stat)) return;
	currentStat = stat;
	stat->OnHPChangedDelegate.AddUObject(this, &UHPBarWidget::UpdateHP);
	float InitPercent = stat->GetCurrentHP() / stat->GetMaxHP();
	UpdateHP(InitPercent);
}

void UHPBarWidget::UpdateHP(float percent)
{
	if (!PB_HPBar) return;
	UE_LOG(LogTemp, Warning, TEXT("Percent: %f"), currentStat->GetCurrentHP() / currentStat->GetMaxHP());
	if (PB_HPBar)
	{
		PB_HPBar->SetPercent(percent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PB_HPBar is NULL!"));
	}
}