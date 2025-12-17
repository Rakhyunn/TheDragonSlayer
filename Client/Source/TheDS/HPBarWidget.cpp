#include "HPBarWidget.h"
#include "Components/ProgressBar.h"
#include "BaseStatComponent.h"

void UHPBarWidget::NativeConstruct()
{
	if (PB_HPBar)
	{
		PB_HPBar->SetPercent(1.0f); // 초기값
	}
}

void UHPBarWidget::BindHp(UBaseStatComponent* Stat)
{
	if (!IsValid(Stat)) return;
	CurrentStat = Stat;
	Stat->OnHPChangedDelegate.AddUObject(this, &UHPBarWidget::UpdateHP);
	float InitPercent = Stat->GetCurrentHP() / Stat->GetMaxHP();
	UpdateHP(InitPercent);
}

void UHPBarWidget::UpdateHP(float Percent)
{
	if (!PB_HPBar) return;
	UE_LOG(LogTemp, Warning, TEXT("Percent: %f"), CurrentStat->GetCurrentHP() / CurrentStat->GetMaxHP());
	if (PB_HPBar)
	{
		PB_HPBar->SetPercent(Percent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PB_HPBar is NULL!"));
	}
}