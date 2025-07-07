// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Hippocache : ModuleRules
{
	public Hippocache(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Public")
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private"),
				Path.Combine(ModuleDirectory, "Private", "Tests")
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine", // For ULocalPlayerSubsystem, UWorld, UGameplayStatics
				"Kismet" // For UBlueprintFunctionLibrary
			}
		);

		// Always include StructUtils for FInstancedStruct support
		PublicDependencyModuleNames.Add("StructUtils");

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"Projects" // For FPlatformProcess::Sleep in tests
			}
		);


		// Enable testing
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"AutomationController"
				}
			);
		}
	}
}
