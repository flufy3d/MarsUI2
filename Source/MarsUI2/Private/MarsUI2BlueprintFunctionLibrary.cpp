#include "MarsUI2PrivatePCH.h"
#include "SBrowserCore.h"
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
void UMarsUI2BlueprintFunctionLibrary::PrepareBrowserCore(const FString& url,bool bTransparency)
{
	IMarsUI2Module::Get().MarsUI2_Static = SNew(SBrowserCore)
		.InitialURL(PrefixProcess(url))
		.ShowControls(false)
		.SupportsTransparency(bTransparency);
	IMarsUI2Module::Get().MarsUI2_Static->Reload();

}
FString UMarsUI2BlueprintFunctionLibrary::PrefixProcess(const FString& NewURL)
{
	if (NewURL.Contains(TEXT("marsui://"), ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{

		// Get the current working directory
		FString GameDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir());

		// We're loading a local file, so replace the proto with our game directory path
		FString LocalFile = NewURL.Replace(TEXT("marsui://"), *GameDir, ESearchCase::IgnoreCase);

		// Now we use the file proto
		LocalFile = FString(TEXT("file:///")) + LocalFile;

		UE_LOG(LogMarsUI2, Log, TEXT("Load Local File: %s"), *LocalFile)

			return LocalFile;
	}

	return NewURL;
}