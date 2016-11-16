// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

// Forward Declarations
class IBrowserCoreSingleton;

/**
 * BrowserCoreModule interface
 */
class IBrowserCoreModule : public IModuleInterface
{
public:
	/**
	 * Get or load the Web Browser Module
	 * 
	 * @return The loaded module
	 */
	static inline IBrowserCoreModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IBrowserCoreModule >("BrowserCore");
	}

	/**
	 * Get the Web Browser Singleton
	 * 
	 * @return The Web Browser Singleton
	 */
	virtual IBrowserCoreSingleton* GetSingleton() = 0;
};