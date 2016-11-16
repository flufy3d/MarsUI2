// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"

#if PLATFORM_ANDROID
#include "AndroidPlatformBrowserCore.h"
#include "AndroidWindow.h"
#include "AndroidJava.h"

class SAndroidBrowserCoreWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SAndroidBrowserCoreWidget)
		: _InitialURL("about:blank")
	{ }

		SLATE_ARGUMENT(FString, InitialURL);
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float) const override;

	void LoadURL(const FString& NewURL);

	void LoadString(const FString& Contents, const FString& DummyURL);

	void ExecuteJavascript(const FString& Script);

	void Close();

protected:
	// mutable to allow calling JWebView_Update from inside OnPaint (which is const)
	mutable TOptional<FJavaClassObject> JWebView;
	TOptional<FJavaClassMethod> JWebView_Update;
	TOptional<FJavaClassMethod> JWebView_LoadURL;
	TOptional<FJavaClassMethod> JWebView_LoadString;
	TOptional<FJavaClassMethod> JWebView_ExecuteJavascript;
	TOptional<FJavaClassMethod> JWebView_Close;
};

void SAndroidBrowserCoreWidget::Construct(const FArguments& Args)
{
	JWebView.Emplace("com/epicgames/ue4/WebViewControl", "()V");

	JWebView_Update = JWebView->GetClassMethod("Update", "(IIII)V");
	JWebView_LoadURL = JWebView->GetClassMethod("LoadURL", "(Ljava/lang/String;)V");
	JWebView_LoadString = JWebView->GetClassMethod("LoadString", "(Ljava/lang/String;Ljava/lang/String;)V");
	JWebView_ExecuteJavascript = JWebView->GetClassMethod("ExecuteJavascript", "(Ljava/lang/String;)V");
	JWebView_Close = JWebView->GetClassMethod("Close", "()V");

	JWebView->CallMethod<void>(JWebView_LoadURL.GetValue(), FJavaClassObject::GetJString(Args._InitialURL));
}

int32 SAndroidBrowserCoreWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Calculate UIScale, which can vary frame-to-frame thanks to device rotation
	// UI Scale is calculated relative to vertical axis of 1280x720 / 720x1280
	float UIScale;
	FPlatformRect ScreenRect = FAndroidWindow::GetScreenRect();
	int32_t ScreenWidth, ScreenHeight;
	FAndroidWindow::CalculateSurfaceSize(FPlatformMisc::GetHardwareWindow(), ScreenWidth, ScreenHeight);
	if (ScreenWidth > ScreenHeight)
	{
		UIScale = (float)ScreenHeight / (ScreenRect.Bottom - ScreenRect.Top);
	}
	else
	{
		UIScale = (float)ScreenHeight / (ScreenRect.Bottom - ScreenRect.Top);
	}

	FVector2D Position = AllottedGeometry.GetAccumulatedRenderTransform().GetTranslation() * UIScale;
	FVector2D Size = TransformVector(AllottedGeometry.GetAccumulatedRenderTransform(), AllottedGeometry.GetLocalSize()) * UIScale;

	// Convert position to integer coordinates
	FIntPoint IntPos(FMath::RoundToInt(Position.X), FMath::RoundToInt(Position.Y));
	// Convert size to integer taking the rounding of position into account to avoid double round-down or double round-up causing a noticeable error.
	FIntPoint IntSize = FIntPoint(FMath::RoundToInt(Position.X + Size.X), FMath::RoundToInt(Size.Y + Position.Y)) - IntPos;

	JWebView->CallMethod<void>(JWebView_Update.GetValue(), IntPos.X, IntPos.Y, IntSize.X, IntSize.Y);

	return LayerId;
}

FVector2D SAndroidBrowserCoreWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(640, 480);
}

void SAndroidBrowserCoreWidget::LoadURL(const FString& NewURL)
{
	JWebView->CallMethod<void>(JWebView_LoadURL.GetValue(), FJavaClassObject::GetJString(NewURL));
}

void SAndroidBrowserCoreWidget::LoadString(const FString& Contents, const FString& DummyURL)
{
	JWebView->CallMethod<void>(JWebView_LoadString.GetValue(), FJavaClassObject::GetJString(Contents), FJavaClassObject::GetJString(DummyURL));
}

void SAndroidBrowserCoreWidget::ExecuteJavascript(const FString& JavaScript)
{
	JWebView->CallMethod<void>(JWebView_ExecuteJavascript.GetValue(), FJavaClassObject::GetJString(JavaScript));
}

void SAndroidBrowserCoreWidget::Close()
{
	JWebView->CallMethod<void>(JWebView_Close.GetValue());
}

FBrowserCoreWindow::FBrowserCoreWindow(FString InUrl, TOptional<FString> InContentsToLoad, bool InShowErrorMessage, bool InThumbMouseButtonNavigation, bool InUseTransparency)
	: CurrentUrl(MoveTemp(InUrl))
	, ContentsToLoad(MoveTemp(InContentsToLoad))
{
}

FBrowserCoreWindow::~FBrowserCoreWindow()
{
	CloseBrowser(true);
}

void FBrowserCoreWindow::LoadURL(FString NewURL)
{
	BrowserWidget->LoadURL(NewURL);
}

void FBrowserCoreWindow::LoadString(FString Contents, FString DummyURL)
{
	BrowserWidget->LoadString(Contents, DummyURL);
}

TSharedRef<SWidget> FBrowserCoreWindow::CreateWidget()
{
	TSharedRef<SAndroidBrowserCoreWidget> BrowserWidgetRef =
		SNew(SAndroidBrowserCoreWidget)
		.InitialURL(CurrentUrl);

	BrowserWidget = BrowserWidgetRef;
	return BrowserWidgetRef;
}

void FBrowserCoreWindow::SetViewportSize(FIntPoint WindowSize, FIntPoint WindowPos)
{
}

FSlateShaderResource* FBrowserCoreWindow::GetTexture(bool bIsPopup /*= false*/)
{
	return nullptr;
}

bool FBrowserCoreWindow::IsValid() const
{
	return false;
}

bool FBrowserCoreWindow::IsInitialized() const
{
	return true;
}

bool FBrowserCoreWindow::IsClosing() const
{
	return false;
}

EBrowserCoreDocumentState FBrowserCoreWindow::GetDocumentLoadingState() const
{
	return EBrowserCoreDocumentState::Loading;
}

FString FBrowserCoreWindow::GetTitle() const
{
	return "";
}

FString FBrowserCoreWindow::GetUrl() const
{
	return "";
}

bool FBrowserCoreWindow::OnKeyDown(const FKeyEvent& InKeyEvent)
{
	return false;
}

bool FBrowserCoreWindow::OnKeyUp(const FKeyEvent& InKeyEvent)
{
	return false;
}

bool FBrowserCoreWindow::OnKeyChar(const FCharacterEvent& InCharacterEvent)
{
	return false;
}

FReply FBrowserCoreWindow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	return FReply::Unhandled();
}

FReply FBrowserCoreWindow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	return FReply::Unhandled();
}

FReply FBrowserCoreWindow::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	return FReply::Unhandled();
}

FReply FBrowserCoreWindow::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	return FReply::Unhandled();
}

FReply FBrowserCoreWindow::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	return FReply::Unhandled();
}

void FBrowserCoreWindow::OnFocus(bool SetFocus, bool bIsPopup)
{
}

void FBrowserCoreWindow::OnCaptureLost()
{
}

bool FBrowserCoreWindow::CanGoBack() const
{
	return false;
}

void FBrowserCoreWindow::GoBack()
{
}

bool FBrowserCoreWindow::CanGoForward() const
{
	return false;
}

void FBrowserCoreWindow::GoForward()
{
}

bool FBrowserCoreWindow::IsLoading() const
{
	return false;
}

void FBrowserCoreWindow::Reload()
{
}

void FBrowserCoreWindow::StopLoad()
{
}

void FBrowserCoreWindow::GetSource(TFunction<void (const FString&)> Callback) const
{
	Callback(FString());
}

int FBrowserCoreWindow::GetLoadError()
{
	return 0;
}

void FBrowserCoreWindow::SetIsDisabled(bool bValue)
{
}


void FBrowserCoreWindow::ExecuteJavascript(const FString& Script)
{
	BrowserWidget->ExecuteJavascript(Script);
}

void FBrowserCoreWindow::CloseBrowser(bool bForce)
{
	BrowserWidget->Close();
}

void FBrowserCoreWindow::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent /*= true*/)
{
}

void FBrowserCoreWindow::UnbindUObject(const FString& Name, UObject* Object /*= nullptr*/, bool bIsPermanent /*= true*/)
{
}

#endif
