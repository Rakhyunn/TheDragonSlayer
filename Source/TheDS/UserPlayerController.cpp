#include "UserPlayerController.h"
#include "PlayerInfoWidget.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"

void AUserPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerInfoWidgetClass)
	{
		PlayerInfoWidget = CreateWidget<UPlayerInfoWidget>(this, PlayerInfoWidgetClass);
		if (PlayerInfoWidget)
		{
			PlayerInfoWidget->AddToViewport();

			// 캐릭터에서 스탯 컴포넌트 가져와 바인딩
			APawn* MyPawn = GetPawn();
			ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
			if (MyCharacter && MyCharacter->stat)
			{
				PlayerInfoWidget->BindInfo(MyCharacter->stat);
			}
		}
	}
}
