// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MarsUI2PrivatePCH.h"

//////////////////////////////////////////////////////////////////////////
// FMarsUI2Module

class FMarsUI2Module : public IMarsUI2Module
{
public:
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}

};


TSharedPtr<class SBrowserCore> IMarsUI2Module::MarsUI2_Static;

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MODULE(FMarsUI2Module, MarsUI2);
DEFINE_LOG_CATEGORY(LogMarsUI2);
