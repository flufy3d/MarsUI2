// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BrowserCore : ModuleRules
{
	public BrowserCore(TargetInfo Target)
	{
		PublicIncludePaths.Add("Runtime/BrowserCore/Public");
		PrivateIncludePaths.Add("Runtime/BrowserCore/Private");

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"RHI",
				"InputCore",
				"Slate",
				"SlateCore",
				"Serialization",
				"CEF3Utils",
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Win64
		||  Target.Platform == UnrealTargetPlatform.Win32
		||  Target.Platform == UnrealTargetPlatform.Mac)
		{
			AddEngineThirdPartyPrivateStaticDependencies(Target,
				"CEF3"
				);

			if (Target.Platform == UnrealTargetPlatform.Mac)
			{
				// Add contents of UnrealCefSubProcess.app directory as runtime dependencies
				foreach (string FilePath in Directory.EnumerateFiles(BuildConfiguration.RelativeEnginePath + "/Binaries/Mac/UnrealCEFSubProcess.app", "*", SearchOption.AllDirectories))
				{
					RuntimeDependencies.Add(new RuntimeDependency(FilePath));
				}
			}
			else
			{
				RuntimeDependencies.Add(new RuntimeDependency("$(EngineDir)/Binaries/" + Target.Platform.ToString() + "/UnrealCEFSubProcess.exe"));
			}
		}

		if (Target.Platform == UnrealTargetPlatform.PS4)
		{
			PrivateDependencyModuleNames.Add("OnlineSubsystem");
		}
		
		bEnableShadowVariableWarnings = false;
	}
}
