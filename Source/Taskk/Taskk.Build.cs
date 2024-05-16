// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Taskk : ModuleRules
{
	public Taskk(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"EnhancedInput",
				"UMG",
				"UMGEditor",
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		PrivateDependencyModuleNames.AddRange(new[] { "UnrealEd", "Blutility", "UMGEditor", "SourceControl", "GraphEditor" });
		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "SlateCore", "Niagara", "Steamworks"});
		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "UMG", "Slate", "SlateCore", "MutableRuntime" });


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
		
		PrivateIncludePaths.Add("Taskk");
	}
}