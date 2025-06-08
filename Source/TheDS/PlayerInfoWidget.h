#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWidget.generated.h"

UCLASS()
class THEDS_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	TWeakObjectPtr<class UBaseStatComponent> currentStat;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_HP;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_MP;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_EXP;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_MaxHP;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_MaxMP;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_MaxExp;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_CurHP;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_CurMP;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_CurExp;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Level;

protected:
	virtual void NativeConstruct() override;

public:
	void BindInfo(class UBaseStatComponent* stat);

	UFUNCTION()
	void UpdateHP(float percent);

	UFUNCTION()
	void UpdateMP(float percent);

	UFUNCTION()
	void UpdateEXP(float percent);

	UFUNCTION()
	void UpdateLevel(int32 newLevel);
};
