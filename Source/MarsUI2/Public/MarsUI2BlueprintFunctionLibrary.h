#pragma once

#include "MarsUI2BlueprintFunctionLibrary.generated.h"

UCLASS(ClassGroup = MarsUI2, Blueprintable)
class MARSUI2_API UMarsUI2BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{

    GENERATED_UCLASS_BODY()


    UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Create My Obj", CompactNodeTitle = "MYOBJECT", Keywords = "Create My Obj"), Category = MarsUI2)
        static UMyObject* NewMyObject(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clear BrowserCore", Keywords = "Clear BrowserCore"), Category = MarsUI2)
		static void ClearBrowserCore();
};