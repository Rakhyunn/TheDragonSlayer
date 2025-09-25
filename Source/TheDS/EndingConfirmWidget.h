#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingConfirmWidget.generated.h"

UCLASS()
class THEDS_API UEndingConfirmWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Info;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Accept;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Decline;

	class ADragonBoss* Dragon;
	bool bIsLeader = false;

protected:
	virtual void NativeConstruct() override;

public:
	void Init(class ADragonBoss* Dragon_, bool bIsLeader_);

	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnDeclineClicked();
};
