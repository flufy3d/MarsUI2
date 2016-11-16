// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IBrowserCoreSingleton.h"

#if WITH_CEF3
#if PLATFORM_WINDOWS
	#include "AllowWindowsPlatformTypes.h"
#endif
#pragma push_macro("OVERRIDE")
#undef OVERRIDE // cef headers provide their own OVERRIDE macro
#include "include/internal/cef_ptr.h"
#include "include/cef_request_context.h"
#pragma pop_macro("OVERRIDE")
#if PLATFORM_WINDOWS
	#include "HideWindowsPlatformTypes.h"
#endif

class CefListValue;
#endif


class FBrowserCoreApp;
class FBrowserCoreHandler;
class FBrowserCoreWindow;

PRAGMA_DISABLE_DEPRECATION_WARNINGS

/**
 * Implementation of singleton class that takes care of general web browser tasks
 */
class FBrowserCoreSingleton
	: public IBrowserCoreSingleton
	, public FTickerObjectBase
{
public:

	/** Default constructor. */
	FBrowserCoreSingleton();

	/** Virtual destructor. */
	virtual ~FBrowserCoreSingleton();

	/**
	* Gets the Current Locale Code in the format CEF expects
	*
	* @return Locale code as either "xx" or "xx-YY"
	*/
	static FString GetCurrentLocaleCode();

public:

	// IBrowserCoreSingleton Interface

	virtual TSharedRef<IBrowserCoreWindowFactory> GetBrowserCoreWindowFactory() const override;

	TSharedPtr<IBrowserCoreWindow> CreateBrowserWindow(
		TSharedPtr<FBrowserCoreWindow>& BrowserWindowParent,
		TSharedPtr<FBrowserCoreWindowInfo>& BrowserWindowInfo) override;

	TSharedPtr<IBrowserCoreWindow> CreateBrowserWindow(
		void* OSWindowHandle, 
		FString InitialURL, 
		bool bUseTransparency,
		bool bThumbMouseButtonNavigation,
		TOptional<FString> ContentsToLoad = TOptional<FString>(), 
		bool ShowErrorMessage = true,
		FColor BackgroundColor = FColor(255, 255, 255, 255),
		int BrowserFrameRate = 24 ) override;

	TSharedPtr<IBrowserCoreWindow> CreateBrowserWindow(const FCreateBrowserWindowSettings& Settings) override;

	virtual void DeleteBrowserCookies(FString URL = TEXT(""), FString CookieName = TEXT(""), TFunction<void(int)> Completed = nullptr) override;

	virtual TSharedPtr<IBrowserCoreCookieManager> GetCookieManager() const override
	{
		return DefaultCookieManager;
	}

	virtual TSharedPtr<IBrowserCoreCookieManager> GetCookieManager(TOptional<FString> ContextId) const override;

	virtual bool RegisterContext(const FBrowserContextSettings& Settings) override;

	virtual bool UnregisterContext(const FString& ContextId) override;

	virtual bool IsDevToolsShortcutEnabled() override
	{
		return bDevToolsShortcutEnabled;
	}

	virtual void SetDevToolsShortcutEnabled(bool Value) override
	{
		bDevToolsShortcutEnabled = Value;
	}

public:

	// FTickerObjectBase Interface

	virtual bool Tick(float DeltaTime) override;

private:

	TSharedPtr<IBrowserCoreCookieManager> DefaultCookieManager;

#if WITH_CEF3
	/** When new render processes are created, send all permanent variable bindings to them. */
	void HandleRenderProcessCreated(CefRefPtr<CefListValue> ExtraInfo);
	/** Pointer to the CEF App implementation */
	CefRefPtr<FBrowserCoreApp>			BrowserCoreApp;
	/** List of currently existing browser windows */
	TArray<TWeakPtr<FBrowserCoreWindow>>	WindowInterfaces;

	TMap<FString, CefRefPtr<CefRequestContext>> RequestContexts;
#endif

	TSharedRef<IBrowserCoreWindowFactory> BrowserCoreWindowFactory;

	bool bDevToolsShortcutEnabled;

};

PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_CEF3

class CefCookieManager;

class FCefBrowserCoreCookieManagerFactory
{
public:
	static TSharedRef<IBrowserCoreCookieManager> Create(
		const CefRefPtr<CefCookieManager>& CookieManager);
};

#endif