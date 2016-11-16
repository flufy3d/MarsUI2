// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"
#include "BrowserCoreViewport.h"
#include "IBrowserCoreWindow.h"

FIntPoint FBrowserCoreViewport::GetSize() const
{
	return (BrowserCoreWindow->GetTexture(bIsPopup) != nullptr)
		? FIntPoint(BrowserCoreWindow->GetTexture(bIsPopup)->GetWidth(), BrowserCoreWindow->GetTexture(bIsPopup)->GetHeight())
		: FIntPoint();
}

FSlateShaderResource* FBrowserCoreViewport::GetViewportRenderTargetTexture() const
{
	return BrowserCoreWindow->GetTexture(bIsPopup);
}

void FBrowserCoreViewport::Tick( const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime )
{
	if (!bIsPopup)
	{
		// Calculate max corner of the viewport using same method as Slate
		FVector2D MaxPos = AllottedGeometry.AbsolutePosition + AllottedGeometry.GetLocalSize();
		// Get size by subtracting as int to avoid incorrect rounding when size and position are .5
		BrowserCoreWindow->SetViewportSize(MaxPos.IntPoint() - AllottedGeometry.AbsolutePosition.IntPoint(), AllottedGeometry.AbsolutePosition.IntPoint());
	}
}

bool FBrowserCoreViewport::RequiresVsync() const
{
	return false;
}

bool FBrowserCoreViewport::AllowScaling() const
{
	return false;
}

FCursorReply FBrowserCoreViewport::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent )
{
	return BrowserCoreWindow->OnCursorQuery(MyGeometry, CursorEvent);
}

FReply FBrowserCoreViewport::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Capture mouse on left button down so that you can drag out of the viewport
	FReply Reply = BrowserCoreWindow->OnMouseButtonDown(MyGeometry, MouseEvent, bIsPopup);
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const FWidgetPath* Path = MouseEvent.GetEventPath();
		if (Path->IsValid())
		{
			TSharedRef<SWidget> TopWidget = Path->Widgets.Last().Widget;
			return Reply.CaptureMouse(TopWidget);
		}
	}
	return Reply;
}

FReply FBrowserCoreViewport::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Release mouse capture when left button released
	FReply Reply = BrowserCoreWindow->OnMouseButtonUp(MyGeometry, MouseEvent, bIsPopup);
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.ReleaseMouseCapture();
	}
	return Reply;
}

void FBrowserCoreViewport::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
}

void FBrowserCoreViewport::OnMouseLeave(const FPointerEvent& MouseEvent)
{
}

FReply FBrowserCoreViewport::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return BrowserCoreWindow->OnMouseMove(MyGeometry, MouseEvent, bIsPopup);
}

FReply FBrowserCoreViewport::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return BrowserCoreWindow->OnMouseWheel(MyGeometry, MouseEvent, bIsPopup);
}

FReply FBrowserCoreViewport::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = BrowserCoreWindow->OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent, bIsPopup);
	return Reply;
}

FReply FBrowserCoreViewport::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return BrowserCoreWindow->OnKeyDown(InKeyEvent) ? FReply::Handled() : FReply::Unhandled();
}

FReply FBrowserCoreViewport::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return BrowserCoreWindow->OnKeyUp(InKeyEvent) ? FReply::Handled() : FReply::Unhandled();
}

FReply FBrowserCoreViewport::OnKeyChar( const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent )
{
	return BrowserCoreWindow->OnKeyChar(InCharacterEvent) ? FReply::Handled() : FReply::Unhandled();
}

FReply FBrowserCoreViewport::OnFocusReceived(const FFocusEvent& InFocusEvent)
{
	BrowserCoreWindow->OnFocus(true, bIsPopup);
	return FReply::Handled();
}

void FBrowserCoreViewport::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	BrowserCoreWindow->OnFocus(false, bIsPopup);
}
