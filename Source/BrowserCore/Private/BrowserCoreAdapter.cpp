// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"
#include "IBrowserCoreAdapter.h"
#include "IBrowserCoreWindow.h"
#include "CoreUObject.h"
#include "GCObject.h"

class FDefaultBrowserCoreAdapter
	: public IBrowserCoreAdapter
	, public FGCObject

{
public:

	virtual FString GetName() const override
	{
		return Name;
	}

	virtual bool IsPermanent() const override
	{
		return bIsPermanent;
	}

	virtual void ConnectTo(const TSharedRef<IBrowserCoreWindow>& BrowserWindow) override
	{
		if (JSBridge != nullptr)
		{
			BrowserWindow->BindUObject(Name, JSBridge, bIsPermanent);
		}

		if (!ConnectScriptText.IsEmpty())
		{
			BrowserWindow->ExecuteJavascript(ConnectScriptText);
		}
	}

	virtual void DisconnectFrom(const TSharedRef<IBrowserCoreWindow>& BrowserWindow) override
	{
		if (!DisconnectScriptText.IsEmpty())
		{
			BrowserWindow->ExecuteJavascript(DisconnectScriptText);
		}

		if (JSBridge != nullptr)
		{
			BrowserWindow->UnbindUObject(Name, JSBridge, bIsPermanent);
		}
	}

	// FGCObject API
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		if (JSBridge != nullptr)
		{
			Collector.AddReferencedObject(JSBridge);
		}
	}

private:

	FDefaultBrowserCoreAdapter(
		const FString InName,
		const FString InConnectScriptText,
		const FString InDisconnectScriptText,
		UObject* InJSBridge,
		const bool InIsPermanent)
		: Name(InName)
		, ConnectScriptText(InConnectScriptText)
		, DisconnectScriptText(InDisconnectScriptText)
		, JSBridge(InJSBridge)
		, bIsPermanent(InIsPermanent)
	{ }

private:

	const FString Name;
	const FString ConnectScriptText;
	const FString DisconnectScriptText;

	UObject* JSBridge;

	const bool bIsPermanent;

	friend FBrowserCoreAdapterFactory;
};

TSharedRef<IBrowserCoreAdapter> FBrowserCoreAdapterFactory::Create(const FString& Name, UObject* JSBridge, bool IsPermanent)
{
	return MakeShareable(new FDefaultBrowserCoreAdapter(Name, FString(), FString(), JSBridge, IsPermanent));
}

TSharedRef<IBrowserCoreAdapter> FBrowserCoreAdapterFactory::Create(const FString& Name, UObject* JSBridge, bool IsPermanent, const FString& ConnectScriptText, const FString& DisconnectScriptText)
{
	return MakeShareable(new FDefaultBrowserCoreAdapter(Name, ConnectScriptText, DisconnectScriptText, JSBridge, IsPermanent));
}