// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"
#include "BrowserCoreApp.h"

#if WITH_CEF3
FBrowserCoreApp::FBrowserCoreApp()
{
}

void FBrowserCoreApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> CommandLine)
{
}

void FBrowserCoreApp::OnBeforeCommandLineProcessing(const CefString& ProcessType, CefRefPtr< CefCommandLine > CommandLine)
{
	CommandLine->AppendSwitch("disable-gpu");
	CommandLine->AppendSwitch("disable-gpu-compositing");
	CommandLine->AppendSwitch("enable-begin-frame-scheduling");
}

void FBrowserCoreApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> ExtraInfo)
{
	RenderProcessThreadCreatedDelegate.ExecuteIfBound(ExtraInfo);
}

#endif
