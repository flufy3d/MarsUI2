#include "MarsUI2PrivatePCH.h"
#include "MarsUI2BlueprintFunctionLibrary.h"
#include "MyObject.h"


UMarsUI2BlueprintFunctionLibrary::UMarsUI2BlueprintFunctionLibrary(const class FObjectInitializer& PCIP)
: Super(PCIP)
{

}


UMyObject* UMarsUI2BlueprintFunctionLibrary::NewMyObject(UObject* WorldContextObject)
{

    UMyObject* tempObj = NewObject<UMyObject>(GetTransientPackage(), UMyObject::StaticClass());

    return tempObj;

}
void UMarsUI2BlueprintFunctionLibrary::ClearBrowserCore()
{
	IMarsUI2Module::Get().MarsUI2_Static.Reset();
}