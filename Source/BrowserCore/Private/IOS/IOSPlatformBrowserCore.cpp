// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"

#if PLATFORM_IOS
#include "IOSPlatformBrowserCore.h"
#include "IOSView.h"
#include "IOSAppDelegate.h"

#import <UIKit/UIKit.h>

@implementation IOSWebViewWrapper

#if !PLATFORM_TVOS
@synthesize WebView;
#endif
@synthesize NextURL;
@synthesize NextContent;

-(void)create:(TSharedPtr<SIOSBrowserCoreWidget>)InBrowserCoreWidget;
{
	BrowserCoreWidget = InBrowserCoreWidget;
	NextURL = nil;
	NextContent = nil;
	bNeedsAddToView = true;

#if !PLATFORM_TVOS
	dispatch_async(dispatch_get_main_queue(), ^
	{
		WebView = [[UIWebView alloc]initWithFrame:CGRectMake(1, 1, 100, 100)];
		WebView.delegate = self;
	});
#endif
}

-(void)close;
{
#if !PLATFORM_TVOS
	WebView.delegate = nil;
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[self.WebView removeFromSuperview];
		WebView = nil;
	});
#endif
}

-(void)updateframe:(CGRect)InFrame;
{
	self.DesiredFrame = InFrame;

#if !PLATFORM_TVOS
	dispatch_async(dispatch_get_main_queue(), ^
	{
		if(WebView != nil)
		{
			WebView.frame = self.DesiredFrame;
			if(bNeedsAddToView)
			{
				bNeedsAddToView = false;
				[[IOSAppDelegate GetDelegate].IOSView addSubview:WebView];
			}
			else
			{
				if(NextContent != nil)
				{
					// Load web content from string
					[self.WebView loadHTMLString:NextContent baseURL:NextURL];
					NextContent = nil;
					NextURL = nil;
				}
				else
				if(NextURL != nil)
				{
					// Load web content from URL
					NSURLRequest *nsrequest = [NSURLRequest requestWithURL:NextURL];
					[self.WebView loadRequest : nsrequest];
					NextURL = nil;
				}
			}
		}
	});
#endif
}

-(void)executejavascript:(NSString*)InJavaScript
{
#if !PLATFORM_TVOS
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[self.WebView stringByEvaluatingJavaScriptFromString:InJavaScript];
	});
#endif
}

-(void)loadurl:(NSURL*)InURL;
{
	NextURL = InURL;
}

-(void)loadstring:(NSString*)InString dummyurl:(NSURL*)InURL;
{
	NextContent = InString;
	NextURL = InURL;
}

@end

class SIOSBrowserCoreWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SIOSBrowserCoreWidget)
		: _InitialURL("about:blank")
	{ }
		SLATE_ARGUMENT(FString, InitialURL);
	SLATE_END_ARGS()

	SIOSBrowserCoreWidget()
	: WebViewWrapper(nil)
	{}

	void Construct(const FArguments& Args)
	{
		WebViewWrapper = [IOSWebViewWrapper alloc];
		[WebViewWrapper create: TSharedPtr<SIOSBrowserCoreWidget>(this)];
		LoadURL(Args._InitialURL);
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		if(WebViewWrapper != nil)
		{
			UIView* View = [IOSAppDelegate GetDelegate].IOSView;
			CGFloat contentScaleFactor = View.contentScaleFactor;
			FVector2D Position = AllottedGeometry.GetAccumulatedRenderTransform().GetTranslation() / contentScaleFactor;
			FVector2D Size = TransformVector(AllottedGeometry.GetAccumulatedRenderTransform(), AllottedGeometry.GetLocalSize()) / contentScaleFactor;
			CGRect NewFrame;
			NewFrame.origin.x = FMath::RoundToInt(Position.X);
			NewFrame.origin.y = FMath::RoundToInt(Position.Y);
			NewFrame.size.width = FMath::RoundToInt(Size.X);
			NewFrame.size.height = FMath::RoundToInt(Size.Y);

			[WebViewWrapper updateframe:NewFrame];
		}

		return LayerId;
	}

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		return FVector2D(640, 480);
	}

	void LoadURL(const FString& InNewURL)
	{
		if( WebViewWrapper != nil)
		{
			[WebViewWrapper loadurl:[NSURL URLWithString:[NSString stringWithUTF8String:TCHAR_TO_UTF8(*InNewURL)]]];
		}
	}

	void LoadString(const FString& InContents, const FString& InDummyURL)
	{
		if( WebViewWrapper != nil)
		{
			[WebViewWrapper loadstring:[NSString stringWithUTF8String:TCHAR_TO_UTF8(*InContents)] dummyurl:[NSURL URLWithString:[NSString stringWithUTF8String:TCHAR_TO_UTF8(*InDummyURL)]]];
		}
	}
	
	void ExecuteJavascript(const FString& Script)
	{
		if (WebViewWrapper != nil)
		{
			[WebViewWrapper executejavascript:[NSString stringWithUTF8String:TCHAR_TO_UTF8(*Script)]];
		}
	}

	void Close()
	{
		if( WebViewWrapper != nil)
		{
			[WebViewWrapper close];
			WebViewWrapper = nil;
		}
	}

	~SIOSBrowserCoreWidget()
	{
		Close();
	}
	
protected:
	mutable __strong IOSWebViewWrapper* WebViewWrapper;
};


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
	TSharedRef<SIOSBrowserCoreWidget> BrowserWidgetRef =
		SNew(SIOSBrowserCoreWidget)
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
