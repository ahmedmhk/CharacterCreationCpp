// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CharacterCreationCpp : ModuleRules
{
	public CharacterCreationCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Paper2D", "GameplayTags" });

		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "EditorStyle", "EditorWidgets", "ToolMenus", "AssetRegistry", "ContentBrowser", "EditorSubsystem", "RenderCore", "ImageWrapper" });
		
		// Add dependencies required for commandlets
		if (Target.Type == TargetType.Editor || Target.Type == TargetType.Program)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "ToolMenus", "Slate", "SlateCore" });
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
