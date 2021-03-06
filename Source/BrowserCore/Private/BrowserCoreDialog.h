// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#if WITH_CEF3
#	if PLATFORM_WINDOWS
#		include "AllowWindowsPlatformTypes.h"
#	endif
#	pragma push_macro("OVERRIDE")
#		undef OVERRIDE // cef headers provide their own OVERRIDE macro
#		include "include/cef_jsdialog_handler.h"
#	pragma pop_macro("OVERRIDE")
#	if PLATFORM_WINDOWS
#		include "HideWindowsPlatformTypes.h"
#	endif
#endif

#include "IBrowserCoreDialog.h"

class FBrowserCoreWindow;

class FBrowserCoreDialog
	: public IBrowserCoreDialog
{
public:
	virtual ~FBrowserCoreDialog()
	{}

	// IBrowserCoreDialog interface:

	virtual EBrowserCoreDialogType GetType() override
	{
		return Type;
	}

	virtual const FText& GetMessageText() override
	{
		return MessageText;
	}

	virtual const FText& GetDefaultPrompt() override
	{
		return DefaultPrompt;
	}

	virtual bool IsReload() override
	{
		check(Type == EBrowserCoreDialogType::Unload);
		return bIsReload;
	}

	virtual void Continue(bool Success = true, const FText& UserResponse = FText::GetEmpty()) override
	{
#if WITH_CEF3
		check(Type == EBrowserCoreDialogType::Prompt || UserResponse.IsEmpty());
		Callback->Continue(Success, *UserResponse.ToString());
#endif
	}

private:


	EBrowserCoreDialogType Type;
	FText MessageText;
	FText DefaultPrompt;
	bool bIsReload;
#if WITH_CEF3
	CefRefPtr<CefJSDialogCallback> Callback;

	// Create a dialog from OnJSDialog arguments
	FBrowserCoreDialog(CefJSDialogHandler::JSDialogType InDialogType, const CefString& InMessageText, const CefString& InDefaultPrompt, CefRefPtr<CefJSDialogCallback> InCallback)
		: Type((EBrowserCoreDialogType)InDialogType)
		, MessageText(FText::FromString(InMessageText.ToWString().c_str()))
		, DefaultPrompt(FText::FromString(InDefaultPrompt.ToWString().c_str()))
		, bIsReload(false)
		, Callback(InCallback)
	{}

	// Create a dialog from OnBeforeUnloadDialog arguments
	FBrowserCoreDialog(const CefString& InMessageText, bool InIsReload, CefRefPtr<CefJSDialogCallback> InCallback)
		: Type(EBrowserCoreDialogType::Unload)
		, MessageText(FText::FromString(InMessageText.ToWString().c_str()))
		, DefaultPrompt(FText::GetEmpty())
		, bIsReload(InIsReload)
		, Callback(InCallback)
	{};

	friend class FBrowserCoreWindow;
#endif
};
