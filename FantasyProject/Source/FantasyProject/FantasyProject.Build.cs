// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

/*public class FantasyProject : ModuleRules
{
	public FantasyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput","PhysicsCore", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}*/


public class FantasyProject : ModuleRules
{
    public FantasyProject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Correzione degli errori C4668 provenienti da TextureShareCore.
        // Il progetto non utilizza Texture Share, quindi le funzioni vengono disattivate.
        PublicDefinitions.Add("TEXTURESHARECORE_SDK=0");
        PublicDefinitions.Add("TEXTURESHARECORE_DEBUGLOG=0");

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "HeadMountedDisplay",
                "EnhancedInput",
                "PhysicsCore",
                "UMG",
                "Niagara",
                "AIModule",
                "GameplayTasks",
                "NavigationSystem",
                "AnimGraphRuntime",
                "Water"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore"
            }
        );
    }
}

