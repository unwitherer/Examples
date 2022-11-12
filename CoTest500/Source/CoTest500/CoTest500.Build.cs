// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CoTest500 : ModuleRules
{
	public CoTest500(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });


        CppStandard = CppStandardVersion.Latest;
    }
}
