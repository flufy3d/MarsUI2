// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"
#include "SBrowserCoreView.h"
#include "SThrobber.h"
#include "BrowserCoreModule.h"
#include "IBrowserCoreSingleton.h"
#include "IBrowserCoreWindow.h"
#include "BrowserCoreViewport.h"
#include "IBrowserCoreAdapter.h"

#if PLATFORM_ANDROID
#include "Android/AndroidPlatformBrowserCore.h"
#elif PLATFORM_IOS
#include "IOS/IOSPlatformBrowserCore.h"
#elif PLATFORM_PS4
#include "PS4/PS4PlatformBrowserCore.h"
#endif

#define LOCTEXT_NAMESPACE "BrowserCore"




SBrowserCoreView::SBrowserCoreView()
{
}

SBrowserCoreView::~SBrowserCoreView()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->OnDocumentStateChanged().RemoveAll(this);
		BrowserWindow->OnNeedsRedraw().RemoveAll(this);
		BrowserWindow->OnTitleChanged().RemoveAll(this);
		BrowserWindow->OnUrlChanged().RemoveAll(this);
		BrowserWindow->OnToolTip().RemoveAll(this);
		BrowserWindow->OnShowPopup().RemoveAll(this);
		BrowserWindow->OnDismissPopup().RemoveAll(this);

		BrowserWindow->OnShowDialog().Unbind();
		BrowserWindow->OnDismissAllDialogs().Unbind();
		BrowserWindow->OnCreateWindow().Unbind();
		BrowserWindow->OnCloseWindow().Unbind();

		if (BrowserWindow->OnBeforeBrowse().IsBoundToObject(this))
		{
			BrowserWindow->OnBeforeBrowse().Unbind();
		}

		if (BrowserWindow->OnLoadUrl().IsBoundToObject(this))
		{
			BrowserWindow->OnLoadUrl().Unbind();
		}

		if (BrowserWindow->OnBeforePopup().IsBoundToObject(this))
		{
			BrowserWindow->OnBeforePopup().Unbind();
		}
	}

	if (SlateParentWindowSetupTickHandle.IsValid())
	{
		FTicker::GetCoreTicker().RemoveTicker(SlateParentWindowSetupTickHandle);
		SlateParentWindowSetupTickHandle.Reset();
	}

	if (SlateParentWindow.IsValid())
	{
		SlateParentWindow->GetOnWindowDeactivatedEvent().RemoveAll(this);
	}
}

void SBrowserCoreView::Construct(const FArguments& InArgs, const TSharedPtr<IBrowserCoreWindow>& InBrowserCoreWindow)
{
	OnLoadCompleted = InArgs._OnLoadCompleted;
	OnLoadError = InArgs._OnLoadError;
	OnLoadStarted = InArgs._OnLoadStarted;
	OnTitleChanged = InArgs._OnTitleChanged;
	OnUrlChanged = InArgs._OnUrlChanged;
	OnBeforeNavigation = InArgs._OnBeforeNavigation;
	OnLoadUrl = InArgs._OnLoadUrl;
	OnShowDialog = InArgs._OnShowDialog;
	OnDismissAllDialogs = InArgs._OnDismissAllDialogs;
	OnBeforePopup = InArgs._OnBeforePopup;
	OnCreateWindow = InArgs._OnCreateWindow;
	OnCloseWindow = InArgs._OnCloseWindow;
	AddressBarUrl = FText::FromString(InArgs._InitialURL);
	PopupMenuMethod = InArgs._PopupMenuMethod;

	BrowserWindow = InBrowserCoreWindow;
	if(!BrowserWindow.IsValid())
	{

		static bool AllowCEF = !FParse::Param(FCommandLine::Get(), TEXT("nocef"));
		if (AllowCEF)
		{
			FCreateBrowserWindowSettings Settings;
			Settings.InitialURL = InArgs._InitialURL;
			Settings.bUseTransparency = InArgs._SupportsTransparency;
			Settings.bThumbMouseButtonNavigation = InArgs._SupportsThumbMouseButtonNavigation;
			Settings.ContentsToLoad = InArgs._ContentsToLoad;
			Settings.bShowErrorMessage = InArgs._ShowErrorMessage;
			Settings.BackgroundColor = InArgs._BackgroundColor;
			Settings.Context = InArgs._ContextSettings;

			BrowserWindow = IBrowserCoreModule::Get().GetSingleton()->CreateBrowserWindow(Settings);		
		}
	}

	SlateParentWindow = InArgs._ParentWindow;

#if WITH_CEF3
	ChildSlot
	[
		SAssignNew(BrowserWidget, SBrowserCoreWidget)
			.ViewportSize(InArgs._ViewportSize)
			.EnableGammaCorrection(false)
			.EnableBlending(InArgs._SupportsTransparency)
			.IgnoreTextureAlpha(!InArgs._SupportsTransparency)
	];
#endif

	if (BrowserWindow.IsValid())
	{
#if PLATFORM_ANDROID || PLATFORM_IOS || PLATFORM_PS4
		// The inner widget creation is handled by the BrowserCoreWindow implementation on mobile.
		const auto& BrowserWidgetRef = static_cast<FBrowserCoreWindow*>(BrowserWindow.Get())->CreateWidget();
		ChildSlot
		[
			BrowserWidgetRef
		];
		BrowserWidget = BrowserWidgetRef;
#endif

		if(OnCreateWindow.IsBound())
		{
			BrowserWindow->OnCreateWindow().BindSP(this, &SBrowserCoreView::HandleCreateWindow);		
		}

		if(OnCloseWindow.IsBound())
		{
			BrowserWindow->OnCloseWindow().BindSP(this, &SBrowserCoreView::HandleCloseWindow);
		}

		BrowserWindow->OnDocumentStateChanged().AddSP(this, &SBrowserCoreView::HandleBrowserWindowDocumentStateChanged);
		BrowserWindow->OnNeedsRedraw().AddSP(this, &SBrowserCoreView::HandleBrowserWindowNeedsRedraw);
		BrowserWindow->OnTitleChanged().AddSP(this, &SBrowserCoreView::HandleTitleChanged);
		BrowserWindow->OnUrlChanged().AddSP(this, &SBrowserCoreView::HandleUrlChanged);
		BrowserWindow->OnToolTip().AddSP(this, &SBrowserCoreView::HandleToolTip);

		if (!BrowserWindow->OnBeforeBrowse().IsBound())
		{
			BrowserWindow->OnBeforeBrowse().BindSP(this, &SBrowserCoreView::HandleBeforeNavigation);
		}
		else
		{
			check(!OnBeforeNavigation.IsBound());
		}

		if (!BrowserWindow->OnLoadUrl().IsBound())
		{
			BrowserWindow->OnLoadUrl().BindSP(this, &SBrowserCoreView::HandleLoadUrl);
		}
		else
		{
			check(!OnLoadUrl.IsBound());
		}

		if (!BrowserWindow->OnBeforePopup().IsBound())
		{
			BrowserWindow->OnBeforePopup().BindSP(this, &SBrowserCoreView::HandleBeforePopup);
		}
		else
		{
			check(!OnBeforePopup.IsBound());
		}

		BrowserWindow->OnShowDialog().BindSP(this, &SBrowserCoreView::HandleShowDialog);
		BrowserWindow->OnDismissAllDialogs().BindSP(this, &SBrowserCoreView::HandleDismissAllDialogs);
		BrowserWindow->OnShowPopup().AddSP(this, &SBrowserCoreView::HandleShowPopup);
		BrowserWindow->OnDismissPopup().AddSP(this, &SBrowserCoreView::HandleDismissPopup);
		BrowserViewport = MakeShareable(new FBrowserCoreViewport(BrowserWindow));
#if WITH_CEF3
		BrowserWidget->SetViewportInterface(BrowserViewport.ToSharedRef());
#endif
		SetupParentWindowHandlers();
		// If we could not obtain the parent window during widget construction, we'll defer and keep trying.
		if (!SlateParentWindow.IsValid())
		{
			SlateParentWindowSetupTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float) -> bool
			{
				this->SetupParentWindowHandlers();
				bool ContinueTick = !SlateParentWindow.IsValid();
				return ContinueTick;
			}));
		}
	}
	else
	{
		OnLoadError.ExecuteIfBound();
	}
}

void SBrowserCoreView::HandleWindowDeactivated()
{
	if (BrowserViewport.IsValid())
	{
		BrowserViewport->OnFocusLost(FFocusEvent());
	}
}

void SBrowserCoreView::LoadURL(FString NewURL)
{
	AddressBarUrl = FText::FromString(NewURL);
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->LoadURL(NewURL);
	}
}

void SBrowserCoreView::LoadString(FString Contents, FString DummyURL)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->LoadString(Contents, DummyURL);
	}
}

void SBrowserCoreView::Reload()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->Reload();
	}
}

void SBrowserCoreView::StopLoad()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->StopLoad();
	}
}

FText SBrowserCoreView::GetTitleText() const
{
	if (BrowserWindow.IsValid())
	{
		return FText::FromString(BrowserWindow->GetTitle());
	}
	return LOCTEXT("InvalidWindow", "Browser Window is not valid/supported");
}

FString SBrowserCoreView::GetUrl() const
{
	if (BrowserWindow.IsValid())
	{
		return BrowserWindow->GetUrl();
	}

	return FString();
}

FText SBrowserCoreView::GetAddressBarUrlText() const
{
	if(BrowserWindow.IsValid())
	{
		return AddressBarUrl;
	}
	return FText::GetEmpty();
}

bool SBrowserCoreView::IsLoaded() const
{
	if (BrowserWindow.IsValid())
	{
		return (BrowserWindow->GetDocumentLoadingState() == EBrowserCoreDocumentState::Completed);
	}

	return false;
}

bool SBrowserCoreView::IsLoading() const
{
	if (BrowserWindow.IsValid())
	{
		return (BrowserWindow->GetDocumentLoadingState() == EBrowserCoreDocumentState::Loading);
	}

	return false;
}

bool SBrowserCoreView::CanGoBack() const
{
	if (BrowserWindow.IsValid())
	{
		return BrowserWindow->CanGoBack();
	}
	return false;
}

void SBrowserCoreView::GoBack()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->GoBack();
	}
}

bool SBrowserCoreView::CanGoForward() const
{
	if (BrowserWindow.IsValid())
	{
		return BrowserWindow->CanGoForward();
	}
	return false;
}

void SBrowserCoreView::GoForward()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->GoForward();
	}
}

bool SBrowserCoreView::IsInitialized() const
{
	return BrowserWindow.IsValid() &&  BrowserWindow->IsInitialized();
}

void SBrowserCoreView::SetupParentWindowHandlers()
{
	if (!SlateParentWindow.IsValid())
	{
		SlateParentWindow = FSlateApplication::Get().FindWidgetWindow(SharedThis(this));
	}

	if (SlateParentWindow.IsValid() && BrowserWindow.IsValid())
	{
		SlateParentWindow->GetOnWindowDeactivatedEvent().AddSP(this, &SBrowserCoreView::HandleWindowDeactivated);
	}
}

void SBrowserCoreView::HandleBrowserWindowDocumentStateChanged(EBrowserCoreDocumentState NewState)
{
	switch (NewState)
	{
	case EBrowserCoreDocumentState::Completed:
		{
			if (BrowserWindow.IsValid())
			{
				for (auto Adapter : Adapters)
				{
					Adapter->ConnectTo(BrowserWindow.ToSharedRef());
				}
			}

			OnLoadCompleted.ExecuteIfBound();
		}
		break;

	case EBrowserCoreDocumentState::Error:
		OnLoadError.ExecuteIfBound();
		break;

	case EBrowserCoreDocumentState::Loading:
		OnLoadStarted.ExecuteIfBound();
		break;
	}
}

void SBrowserCoreView::HandleBrowserWindowNeedsRedraw()
{
	if (FSlateApplication::Get().IsSlateAsleep())
	{
		// Tell slate that the widget needs to wake up for one frame to get redrawn
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateLambda([this](double InCurrentTime, float InDeltaTime) { return EActiveTimerReturnType::Stop; }));
	}
}

void SBrowserCoreView::HandleTitleChanged( FString NewTitle )
{
	const FText NewTitleText = FText::FromString(NewTitle);
	OnTitleChanged.ExecuteIfBound(NewTitleText);
}

void SBrowserCoreView::HandleUrlChanged( FString NewUrl )
{
	AddressBarUrl = FText::FromString(NewUrl);
	OnUrlChanged.ExecuteIfBound(AddressBarUrl);
}

void SBrowserCoreView::HandleToolTip(FString ToolTipText)
{
	if(ToolTipText.IsEmpty())
	{
		FSlateApplication::Get().CloseToolTip();
		SetToolTip(nullptr);
	}
	else
	{
		SetToolTipText(FText::FromString(ToolTipText));
		FSlateApplication::Get().UpdateToolTip(true);
	}
}

bool SBrowserCoreView::HandleBeforeNavigation(const FString& Url, const FWebNavigationRequest& Request)
{
	if(OnBeforeNavigation.IsBound())
	{
		return OnBeforeNavigation.Execute(Url, Request);
	}
	return false;
}

bool SBrowserCoreView::HandleLoadUrl(const FString& Method, const FString& Url, FString& OutResponse)
{
	if(OnLoadUrl.IsBound())
	{
		return OnLoadUrl.Execute(Method, Url, OutResponse);
	}
	return false;
}

EBrowserCoreDialogEventResponse SBrowserCoreView::HandleShowDialog(const TWeakPtr<IBrowserCoreDialog>& DialogParams)
{
	if(OnShowDialog.IsBound())
	{
		return OnShowDialog.Execute(DialogParams);
	}
	return EBrowserCoreDialogEventResponse::Unhandled;
}

void SBrowserCoreView::HandleDismissAllDialogs()
{
	OnDismissAllDialogs.ExecuteIfBound();
}


bool SBrowserCoreView::HandleBeforePopup(FString URL, FString Target)
{
	if (OnBeforePopup.IsBound())
	{
		return OnBeforePopup.Execute(URL, Target);
	}

	return false;
}

void SBrowserCoreView::ExecuteJavascript(const FString& ScriptText)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->ExecuteJavascript(ScriptText);
	}
}

bool SBrowserCoreView::HandleCreateWindow(const TWeakPtr<IBrowserCoreWindow>& NewBrowserWindow, const TWeakPtr<IBrowserCorePopupFeatures>& PopupFeatures)
{
	if(OnCreateWindow.IsBound())
	{
		return OnCreateWindow.Execute(NewBrowserWindow, PopupFeatures);
	}
	return false;
}

bool SBrowserCoreView::HandleCloseWindow(const TWeakPtr<IBrowserCoreWindow>& NewBrowserWindow)
{
	if(OnCloseWindow.IsBound())
	{
		return OnCloseWindow.Execute(NewBrowserWindow);
	}
	return false;
}

void SBrowserCoreView::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->BindUObject(Name, Object, bIsPermanent);
	}
}

void SBrowserCoreView::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->UnbindUObject(Name, Object, bIsPermanent);
	}
}

void SBrowserCoreView::BindAdapter(const TSharedRef<IBrowserCoreAdapter>& Adapter)
{
	Adapters.Add(Adapter);
	if (BrowserWindow.IsValid())
	{
		Adapter->ConnectTo(BrowserWindow.ToSharedRef());
	}
}

void SBrowserCoreView::UnbindAdapter(const TSharedRef<IBrowserCoreAdapter>& Adapter)
{
	Adapters.Remove(Adapter);
	if (BrowserWindow.IsValid())
	{
		Adapter->DisconnectFrom(BrowserWindow.ToSharedRef());
	}
}

void SBrowserCoreView::HandleShowPopup(const FIntRect& PopupSize)
{
	check(!PopupMenuPtr.IsValid())

	TSharedPtr<SViewport> MenuContent;
	SAssignNew(MenuContent, SViewport)
				.ViewportSize(PopupSize.Size())
				.EnableGammaCorrection(false)
				.EnableBlending(false)
				.IgnoreTextureAlpha(true)
				.Visibility(EVisibility::Visible);
	MenuViewport = MakeShareable(new FBrowserCoreViewport(BrowserWindow, true));
	MenuContent->SetViewportInterface(MenuViewport.ToSharedRef());
	FWidgetPath WidgetPath;
	FSlateApplication::Get().GeneratePathToWidgetUnchecked(SharedThis(this), WidgetPath);
	if (WidgetPath.IsValid())
	{
		TSharedRef< SWidget > MenuContentRef = MenuContent.ToSharedRef();
		const FGeometry& BrowserGeometry = WidgetPath.Widgets.Last().Geometry;
		const FVector2D NewPosition = BrowserGeometry.LocalToAbsolute(PopupSize.Min);


		// Open the pop-up. The popup method will be queried from the widget path passed in.
		TSharedPtr<IMenu> NewMenu = FSlateApplication::Get().PushMenu(SharedThis(this), WidgetPath, MenuContentRef, NewPosition, FPopupTransitionEffect( FPopupTransitionEffect::ComboButton ), false);
		NewMenu->GetOnMenuDismissed().AddSP(this, &SBrowserCoreView::HandleMenuDismissed);
		PopupMenuPtr = NewMenu;
	}

}

void SBrowserCoreView::HandleMenuDismissed(TSharedRef<IMenu>)
{
	PopupMenuPtr.Reset();
}

void SBrowserCoreView::HandleDismissPopup()
{
	if (PopupMenuPtr.IsValid())
	{
		PopupMenuPtr.Pin()->Dismiss();
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::SetDirectly);
	}
}


#undef LOCTEXT_NAMESPACE

