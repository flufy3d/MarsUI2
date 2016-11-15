// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MarsUI2PrivatePCH.h"
#include "MyObject.h"


UMyObject::UMyObject( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

float UMyObject::FunctionA(float bInvisibleParameter) {
	int a = 1;
	float ret = bInvisibleParameter * a;

    return ret;
}

