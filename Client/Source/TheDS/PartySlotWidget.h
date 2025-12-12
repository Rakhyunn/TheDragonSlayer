#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartyState.h"
#include "PartySlotWidget.generated.h"

class UTextBlock;

UCLASS()
class THEDS_API UPartySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Leader;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Level;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Nickname;

public:
	void Init(const FPartyMember& MemberData, bool bIsLeader);
};
