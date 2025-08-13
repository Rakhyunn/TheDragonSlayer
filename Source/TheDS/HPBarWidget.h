#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBarWidget.generated.h"

UCLASS()
class THEDS_API UHPBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	TWeakObjectPtr<class UBaseStatComponent> CurrentStat;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_HPBar;

protected:
	virtual void NativeConstruct() override;

public:
	void BindHp(class UBaseStatComponent* Stat);

	UFUNCTION()
	void UpdateHP(float Percent);
};
