// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MarsUI2PrivatePCH.h"
#include "MyObject.h"


UMyObject::UMyObject( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
	UE_LOG(LogMarsUI2, Log, TEXT("UMyObject::UMyObject"));
}

float UMyObject::FunctionA(float bInvisibleParameter) {
	int a = 1;
	float ret = bInvisibleParameter * a;

    return ret;
}

void UMyObject::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogMarsUI2, Log, TEXT("UMyObject::BeginDestroy"));
}
void UMyObject::PostInitProperties()
{
	Super::PostInitProperties();
	UE_LOG(LogMarsUI2, Log, TEXT("UMyObject::PostInitProperties"))
}
