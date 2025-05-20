// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheDS : ModuleRules
{
	public TheDS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NavigationSystem", "AIModule", "Niagara", "Slate", "SlateCore", "UMG", "Networking", "Sockets" });
	}
}
