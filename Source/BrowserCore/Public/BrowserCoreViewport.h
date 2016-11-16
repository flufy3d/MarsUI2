// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateCore.h"

// Forward Declarations
class IBrowserCoreWindow;

/**
 * A Slate viewport to display a Web Browser Window
 */
class BROWSERCORE_API FBrowserCoreViewport : public ISlateViewport
{
public:
	/**
	 * Default Constructor
	 * 
	 * @param InBrowserCoreWindow The Web Browser Window this viewport will display
	 * @param InViewportWidget The Widget displaying this viewport (needed to capture mouse)
	 * @param InIsPopup Used to initialize a viewport for showing browser popup menus instead of the main window.
	 */
	FBrowserCoreViewport(TSharedPtr<IBrowserCoreWindow> InBrowserCoreWindow, bool InIsPopup = false)
		: BrowserCoreWindow(InBrowserCoreWindow)
		, bIsPopup(InIsPopup)
	{ }

	/**
	 * Destructor.
	 */
	~FBrowserCoreViewport( )
	{
	}

	// ISlateViewport interface
	virtual FIntPoint GetSize() const override;
	virtual FSlateShaderResource* GetViewportRenderTargetTexture() const override;
	virtual void Tick( const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime ) override;
	virtual bool RequiresVsync() const override;
	virtual bool AllowScaling() const override;
	virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
	virtual FReply OnKeyUp( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
	virtual FReply OnKeyChar( const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent ) override;
	virtual FReply OnFocusReceived( const FFocusEvent& InFocusEvent ) override;
	virtual void OnFocusLost( const FFocusEvent& InFocusEvent ) override;

private:
	/** The web browser this viewport will display */
	TSharedPtr<IBrowserCoreWindow>	BrowserCoreWindow;
	/** Whether this viewport is showing the browser window or a popup menu widget */
	bool const						bIsPopup;
};
