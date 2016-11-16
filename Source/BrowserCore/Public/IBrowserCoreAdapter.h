// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

class IBrowserCoreWindow;

class IBrowserCoreAdapter
{
public:

	virtual FString GetName() const = 0;

	virtual bool IsPermanent() const = 0;

	virtual void ConnectTo(const TSharedRef<IBrowserCoreWindow>& BrowserWindow) = 0;

	virtual void DisconnectFrom(const TSharedRef<IBrowserCoreWindow>& BrowserWindow) = 0;

};

class BROWSERCORE_API FBrowserCoreAdapterFactory 
{ 
public: 

	static TSharedRef<IBrowserCoreAdapter> Create(const FString& Name, UObject* JSBridge, bool IsPermanent); 

	static TSharedRef<IBrowserCoreAdapter> Create(const FString& Name, UObject* JSBridge, bool IsPermanent, const FString& ConnectScriptText, const FString& DisconnectScriptText);
}; 