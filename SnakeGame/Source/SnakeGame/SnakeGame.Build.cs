// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SnakeGame : ModuleRules
{
	public SnakeGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { 
            "EnhancedInput",
            "GameDevToolboxCore",
            "GameDevToolboxUI",
            "UMG",
            "CommonUI",
            "Foliage",
            "Niagara"
        });

		PrivateIncludePaths.Add("./SnakeGame/Include/");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
    }
}
