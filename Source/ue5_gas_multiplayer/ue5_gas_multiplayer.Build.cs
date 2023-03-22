// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ue5_gas_multiplayer : ModuleRules
{
	public ue5_gas_multiplayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
