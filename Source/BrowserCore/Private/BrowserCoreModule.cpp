// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"
#include "BrowserCoreSingleton.h"
#include "ModuleManager.h"
#include "CEF3Utils.h"

DEFINE_LOG_CATEGORY(LogBrowserCore);

static FBrowserCoreSingleton* BrowserCoreSingleton = nullptr;

class FBrowserCoreModule : public IBrowserCoreModule
{
private:
	// IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	virtual IBrowserCoreSingleton* GetSingleton() override;
};

IMPLEMENT_MODULE( FBrowserCoreModule, BrowserCore );

void FBrowserCoreModule::StartupModule()
{
#if WITH_CEF3
	CEF3Utils::LoadCEF3Modules();
#endif

	BrowserCoreSingleton = new FBrowserCoreSingleton();
}

void FBrowserCoreModule::ShutdownModule()
{
	delete BrowserCoreSingleton;
	BrowserCoreSingleton = NULL;

#if WITH_CEF3
	CEF3Utils::UnloadCEF3Modules();
#endif
}

IBrowserCoreSingleton* FBrowserCoreModule::GetSingleton()
{
	return BrowserCoreSingleton;
}
