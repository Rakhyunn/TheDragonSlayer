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

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Inventory;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Equipment;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VB_PartyContainer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPartySlotWidget> PartySlotClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_CreateParty;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_InviteParty;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_LeaveParty;

protected:
	virtual void NativeConstruct() override;

public:
	void BindInfo(class UBaseStatComponent* Stat);

	UFUNCTION()
	void UpdateHP(float Percent);

	UFUNCTION()
	void UpdateMP(float Percent);

	UFUNCTION()
	void UpdateEXP(float Percent);

	UFUNCTION()
	void UpdateLevel(int32 NewLevel);

	UFUNCTION()
	void OpenInventory();

	UFUNCTION()
	void OpenEquipment();

	UFUNCTION()
	void UpdateParty();

	UFUNCTION()
	void OnCreatePartyClicked();

	UFUNCTION()
	void OnInviteClicked();

	UFUNCTION()
	void OnLeavePartyClicked();
};
