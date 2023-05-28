using UnrealBuildTool;

public class SoulsGameEditor : ModuleRules
{
	public SoulsGameEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(
			new string[] 
			{ 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",
				"SoulsGame",
				"Slate",
				"SlateCore"

				// ... add public dependencies that you statically link with here ...   
			}
		);
		
		PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
		// Needed for Niagara
		PublicDependencyModuleNames.AddRange(new string[] { "Niagara", "NiagaraAnimNotifies", "NiagaraEditor" });
		
		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{
				// ... add private dependencies that you statically link with here ...    
				"AppFramework",
				"AssetTools",
				"ContentBrowser",
				"DesktopWidgets",
				"EditorStyle",
				"EditorWidgets",
				"Engine",
				"InputCore",
				"LevelEditor",
				"MainFrame",
				"Projects",
				"PropertyEditor",
				"RHI",
				"RawMesh",
				"RenderCore",
				"TargetPlatform",
				"UnrealEd",
				"ApplicationCore",
				"CurveEditor",
				"Json",
				"SceneOutliner",
				"PropertyPath",
				"MaterialEditor",
				"SourceControl",
				"EditorSubsystem",
				"SoulsGame",
				"AssetRegistry",
			}
		);

		PublicIncludePaths.AddRange(
			new string[]
			{
				"SoulsGameEditor/Public"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[]
			{
				"SoulsGameEditor/Private"
			}
		);
	}
}