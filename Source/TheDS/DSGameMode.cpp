#include "DSGameMode.h"
#include "Warrior.h"
#include "Wizard.h"

ADSGameMode::ADSGameMode() {
	//Spawn Warrior or Wizard
	static ConstructorHelpers::FClassFinder<AWarrior> BP_Warrior(TEXT("/Script/Engine.Blueprint'/Game/BP_Value/BP_Warrior.BP_Warrior_C'"));
	static ConstructorHelpers::FClassFinder<AWizard> BP_Wizard(TEXT("/Script/Engine.Blueprint'/Game/BP_Value/BP_Wizard.BP_Wizard_C'"));
	/*if (BP_Warrior.Succeeded()) {
		DefaultPawnClass = BP_Warrior.Class;
	}*/
	if (BP_Wizard.Succeeded()) {
		DefaultPawnClass = BP_Wizard.Class;
	}
}