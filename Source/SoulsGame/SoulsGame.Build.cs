// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulsGame : ModuleRules
{
	public SoulsGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		// GameplayAbilities
		PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
		// Needed for BindKey
		PublicDependencyModuleNames.AddRange(new string[] { "SlateCore", "Slate" });
		// Needed for Niagara
		PublicDependencyModuleNames.AddRange(new string[] { "Niagara", "NiagaraAnimNotifies" });
		
		// Needed for develeoper settings
		PublicDependencyModuleNames.AddRange(new string[] { "DeveloperSettings" });

		
		// Needed for UI?
		PublicDependencyModuleNames.AddRange(new string[] { "UMG" }); // UI releated stuff
		
		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem"  });
		
		// Json save/load
		PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" });
		

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
