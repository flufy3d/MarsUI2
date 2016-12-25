// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MyObject.generated.h"



UCLASS(Blueprintable, BlueprintType)
class UMyObject : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY()
    FString InaccessibleUProperty;

	UFUNCTION(BlueprintCallable, Category = "Web Browser")
    float FunctionA(float bInvisibleParameter);
private:

	virtual void BeginDestroy() override;
	virtual void PostInitProperties() override;


};


