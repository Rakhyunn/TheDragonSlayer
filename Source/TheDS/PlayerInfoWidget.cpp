#include "PlayerInfoWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BaseStatComponent.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.h"
#include "Components/Button.h"
#include "UserPlayerController.h"

void UPlayerInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Inventory) BTN_Inventory->OnClicked.AddDynamic(this, &UPlayerInfoWidget::OpenInventory);
	if (BTN_Equipment) BTN_Equipment->OnClicked.AddDynamic(this, &UPlayerInfoWidget::OpenEquipment);
}

void UPlayerInfoWidget::BindInfo(UBaseStatComponent* stat)
{
	if (!IsValid(stat)) return;
	currentStat = stat;
	stat->OnHPChangedDelegate.AddUObject(this, &UPlayerInfoWidget::UpdateHP);
	stat->OnMPChangedDelegate.AddUObject(this, &UPlayerInfoWidget::UpdateMP);
	stat->OnEXPChangedDelegate.AddUObject(this, &UPlayerInfoWidget::UpdateEXP);
	stat->OnLevelChangedDelegate.AddUObject(this, &UPlayerInfoWidget::UpdateLevel);

	float initHP = stat->GetMaxHP() > 0 ? stat->GetCurrentHP() / stat->GetMaxHP() : 0.f;
	float initMP = stat->GetMaxMP() > 0 ? stat->GetCurrentMP() / stat->GetMaxMP() : 0.f;
	float initEXP = stat->GetMaxEXP() > 0 ? stat->GetCurrentEXP() / stat->GetMaxEXP() : 0.f;
	int32 initLevel = stat->GetLevel();

	UpdateHP(initHP);
	UpdateMP(initMP);
	UpdateEXP(initEXP);
	UpdateLevel(initLevel);
}

void UPlayerInfoWidget::UpdateHP(float percent)
{
	if (!PB_HP || !TXT_CurHP || !TXT_MaxHP) return;
	PB_HP->SetPercent(percent);
	TXT_CurHP->SetText(FText::AsNumber(FMath::FloorToInt(currentStat->GetCurrentHP())));
	TXT_MaxHP->SetText(FText::AsNumber(FMath::FloorToInt(currentStat->GetMaxHP())));
}

void UPlayerInfoWidget::UpdateMP(float percent)
{
	if (!PB_MP || !TXT_CurMP || !TXT_MaxMP) return;
	PB_MP->SetPercent(percent);
	TXT_CurMP->SetText(FText::AsNumber(FMath::FloorToInt(currentStat->GetCurrentMP())));
	TXT_MaxMP->SetText(FText::AsNumber(FMath::FloorToInt(currentStat->GetMaxMP())));
}

void UPlayerInfoWidget::UpdateEXP(float percent)
{
	if (!PB_EXP || !TXT_CurExp || !TXT_MaxExp) return;
	UE_LOG(LogTemp, Warning, TEXT("EXP: %f"), currentStat->GetCurrentEXP() / currentStat->GetMaxEXP());
	PB_EXP->SetPercent(percent);
	TXT_CurExp->SetText(FText::AsNumber(FMath::FloorToInt(currentStat->GetCurrentEXP())));
	TXT_MaxExp->SetText(FText::AsNumber(FMath::FloorToInt(currentStat->GetMaxEXP())));
}

void UPlayerInfoWidget::UpdateLevel(int32 newLevel)
{
	if (!TXT_Level) return;
	TXT_Level->SetText(FText::AsNumber(currentStat->GetLevel()));
}

void UPlayerInfoWidget::OpenInventory()
{
	AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ToggleInventory();
	}
}

void UPlayerInfoWidget::OpenEquipment()
{
	AUserPlayerController* PC = Cast<AUserPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ToggleEquipment();
	}
}
