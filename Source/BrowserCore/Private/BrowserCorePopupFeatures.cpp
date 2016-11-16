// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BrowserCorePrivatePCH.h"
#include "BrowserCorePopupFeatures.h"

#if WITH_CEF3

FBrowserCorePopupFeatures::FBrowserCorePopupFeatures()
	: X(0)
	, bXSet(false)
	, Y(0)
	, bYSet(false)
	, Width(0)
	, bWidthSet(false)
	, Height(0)
	, bHeightSet(false)
	, bMenuBarVisible(true)
	, bStatusBarVisible(false)
	, bToolBarVisible(true)
	, bLocationBarVisible(true)
	, bScrollbarsVisible(true)
	, bResizable(true)
	, bIsFullscreen(false)
	, bIsDialog(false)
{
}


FBrowserCorePopupFeatures::FBrowserCorePopupFeatures( const CefPopupFeatures& PopupFeatures )
{
	X = PopupFeatures.x;
	bXSet = PopupFeatures.xSet ? true : false;
	Y = PopupFeatures.y;
	bYSet = PopupFeatures.ySet ? true : false;
	Width = PopupFeatures.width;
	bWidthSet = PopupFeatures.widthSet ? true : false;
	Height = PopupFeatures.height;
	bHeightSet = PopupFeatures.heightSet ? true : false;
	bMenuBarVisible = PopupFeatures.menuBarVisible ? true : false;
	bStatusBarVisible = PopupFeatures.statusBarVisible ? true : false;
	bToolBarVisible = PopupFeatures.toolBarVisible ? true : false;
	bLocationBarVisible = PopupFeatures.locationBarVisible ? true : false;
	bScrollbarsVisible = PopupFeatures.scrollbarsVisible ? true : false;
	bResizable = PopupFeatures.resizable ? true : false;
	bIsFullscreen = PopupFeatures.fullscreen ? true : false;
	bIsDialog = PopupFeatures.dialog ? true : false;

	int Count = PopupFeatures.additionalFeatures ? cef_string_list_size(PopupFeatures.additionalFeatures) : 0;
	CefString ListValue;

	for(int ListIdx = 0; ListIdx < Count; ListIdx++) 
	{
		cef_string_list_value(PopupFeatures.additionalFeatures, ListIdx, ListValue.GetWritableStruct());
		AdditionalFeatures.Add(ListValue.ToWString().c_str());
	}

}

FBrowserCorePopupFeatures::~FBrowserCorePopupFeatures()
{
}

int FBrowserCorePopupFeatures::GetX() const 
{
	return X;
}

bool FBrowserCorePopupFeatures::IsXSet() const 
{
	return bXSet;
}

int FBrowserCorePopupFeatures::GetY() const 
{
	return Y;
}

bool FBrowserCorePopupFeatures::IsYSet() const 
{
	return bYSet;
}

int FBrowserCorePopupFeatures::GetWidth() const 
{
	return Width;
}

bool FBrowserCorePopupFeatures::IsWidthSet() const 
{
	return bWidthSet;
}

int FBrowserCorePopupFeatures::GetHeight() const 
{
	return Height;
}

bool FBrowserCorePopupFeatures::IsHeightSet() const 
{
	return bHeightSet;
}

bool FBrowserCorePopupFeatures::IsMenuBarVisible() const 
{
	return bMenuBarVisible;
}

bool FBrowserCorePopupFeatures::IsStatusBarVisible() const 
{
	return bStatusBarVisible;
}

bool FBrowserCorePopupFeatures::IsToolBarVisible() const 
{
	return bToolBarVisible;
}

bool FBrowserCorePopupFeatures::IsLocationBarVisible() const 
{
	return bLocationBarVisible;
}

bool FBrowserCorePopupFeatures::IsScrollbarsVisible() const 
{
	return bScrollbarsVisible;
}

bool FBrowserCorePopupFeatures::IsResizable() const 
{
	return bResizable;
}

bool FBrowserCorePopupFeatures::IsFullscreen() const 
{
	return bIsFullscreen;
}

bool FBrowserCorePopupFeatures::IsDialog() const 
{
	return bIsDialog;
}

TArray<FString> FBrowserCorePopupFeatures::GetAdditionalFeatures() const 
{
	return AdditionalFeatures;
}

#endif


