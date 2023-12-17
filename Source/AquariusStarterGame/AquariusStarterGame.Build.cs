using UnrealBuildTool;

public class AquariusStarterGame : ModuleRules
{
	public AquariusStarterGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				"AquariusStarterGame",
			});

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
            "ModularGameplay",
			"ModularGameplayActors",
            "DataRegistry",
			"GameFeatures",
			"Niagara",
            "AnimGraphRuntime",
			"AnimationLocomotionLibraryRuntime"
        });

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"EnhancedInput",
				"UMG",
				"Slate",
				"SlateCore",
			}
		);
	}
}
