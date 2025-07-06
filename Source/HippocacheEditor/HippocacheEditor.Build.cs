using UnrealBuildTool;

public class HippocacheEditor : ModuleRules
{
	public HippocacheEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Hippocache"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ToolMenus",
				"UnrealEd",
				"EditorWidgets",
				"PropertyEditor",
				"SlateCore",
				"Slate"
			}
		);
	}
}