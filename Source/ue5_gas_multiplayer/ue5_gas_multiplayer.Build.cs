// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ue5_gas_multiplayer : ModuleRules
{
	public ue5_gas_multiplayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
		
		PublicIncludePaths.Add("ue5_gas_multiplayer/");
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});
	}
}
