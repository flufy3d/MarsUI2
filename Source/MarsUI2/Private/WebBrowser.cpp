// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MarsUI2PrivatePCH.h"

#include "SBrowserCore.h"

#include "WebBrowser.h"

#include "MyObject.h"

#define LOCTEXT_NAMESPACE "WebBrowser"

/////////////////////////////////////////////////////
// UWebBrowser

UWebBrowser::UWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UWebBrowser::LoadURL(FString NewURL)
{
	if ( MarsUI2.IsValid() )
	{
		return MarsUI2->LoadURL(UMarsUI2BlueprintFunctionLibrary::PrefixProcess(NewURL));
	}
}

void UWebBrowser::LoadString(FString Contents, FString DummyURL)
{
	if ( MarsUI2.IsValid() )
	{
		return MarsUI2->LoadString(Contents, DummyURL);
	}
}

void UWebBrowser::ExecuteJavascript(const FString& ScriptText)
{
	if (MarsUI2.IsValid())
	{
		return MarsUI2->ExecuteJavascript(ScriptText);
	}
}

FText UWebBrowser::GetTitleText() const
{
	if ( MarsUI2.IsValid() )
	{
		return MarsUI2->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UWebBrowser::GetUrl() const
{
	if (MarsUI2.IsValid())
	{
		return MarsUI2->GetUrl();
	}

	return FString();
}

void UWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MarsUI2.Reset();
}

TSharedRef<SWidget> UWebBrowser::RebuildWidget()
{
	if ( IsDesignTime() )
	{
		return BuildDesignTimeWidget(SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Web Browser", "Web Browser"))
			]);
	}
	else
	{
		if (bSupportsPersistent)
		{
			if (IMarsUI2Module::Get().MarsUI2_Static.IsValid())
			{
				MarsUI2 = IMarsUI2Module::Get().MarsUI2_Static;
				MarsUI2->OnUrlChanged = BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged);
				FSlateApplication::Get().ClearAllUserFocus();
			}
			else
			{
				MarsUI2 = SNew(SBrowserCore)
					.InitialURL(UMarsUI2BlueprintFunctionLibrary::PrefixProcess(InitialURL))
					.ShowControls(false)
					.SupportsTransparency(bSupportsTransparency)
					.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged));
				IMarsUI2Module::Get().MarsUI2_Static = MarsUI2;
			}
		}
		else
		{
			MarsUI2 = SNew(SBrowserCore)
				.InitialURL(UMarsUI2BlueprintFunctionLibrary::PrefixProcess(InitialURL))
				.ShowControls(false)
				.SupportsTransparency(bSupportsTransparency)
				.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged));
		}




		return MarsUI2.ToSharedRef();
	}
}


void UWebBrowser::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (MarsUI2.IsValid())
	{
		if (Object != NULL)
		{
			MarsUI2->BindUObject(Name, Object, bIsPermanent);

		}

	}
}


void UWebBrowser::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (MarsUI2.IsValid())
	{
		if (Object != NULL)
		{
			MarsUI2->UnbindUObject(Name, Object, bIsPermanent);
		}

	}
}

UObject * UWebBrowser::CreateInstanceFromClass(TSubclassOf<UObject> SourceClass)
{
	return StaticConstructObject_Internal(SourceClass);
}


void UWebBrowser::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if ( MarsUI2.IsValid() )
	{

	}
}

void UWebBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

#if WITH_EDITOR

const FText UWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Misc", "Misc");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
