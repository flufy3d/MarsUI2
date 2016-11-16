// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widget.h"

#include "WebBrowser.generated.h"

/**
 * 
 */
UCLASS()
class MARSUI2_API UWebBrowser : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);

	/**
	 * Load the specified URL
	 *
	 * @param NewURL New URL to load
	 */
	UFUNCTION(BlueprintCallable, Category="Web Browser")
	void LoadURL(FString NewURL);


	/**
	* Expose a UObject instance to the browser runtime.
	* Properties and Functions will be accessible from JavaScript side.
	* As all communication with the rendering procesis asynchronous, return values (both for properties and function results) are wrapped into JS Future objects.
	*
	* @param Name The name of the object. The object will show up as window.ue4.{Name} on the javascript side. If there is an existing object of the same name, this object will replace it. If bIsPermanent is false and there is an existing permanent binding, the permanent binding will be restored when the temporary one is removed.
	* @param Object The object instance.
	* @param bIsPermanent If true, the object will be visible to all pages loaded through this browser widget, otherwise, it will be deleted when navigating away from the current page. Non-permanent bindings should be registered from inside an OnLoadStarted event handler in order to be available before JS code starts loading.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	void BindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);

	/**
	* Remove an existing script binding registered by BindUObject.
	*
	* @param Name The name of the object to remove.
	* @param Object The object will only be removed if it is the same object as the one passed in.
	* @param bIsPermanent Must match the bIsPermanent argument passed to BindUObject.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	void UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);


	/**
	* Creates an instance of type which is a subclass of UObject
	*
	* @param SourceClass            A type derived from UObject
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	static UObject * CreateInstanceFromClass(TSubclassOf<UObject> SourceClass);


	/**
	 * Load a string as data to create a web page
	 *
	 * @param Contents String to load
	 * @param DummyURL Dummy URL for the page
	 */
	UFUNCTION(BlueprintCallable, Category="Web Browser")
	void LoadString(FString Contents, FString DummyURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	void ExecuteJavascript(const FString& ScriptText);

	/**
	 * Get the current title of the web page
	 */
	UFUNCTION(BlueprintCallable, Category="Web Browser")
	FText GetTitleText() const;

	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	FString GetUrl() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
	FOnUrlChanged OnUrlChanged;


public:

	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category=Appearance)
	FString InitialURL;

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category=Appearance)
	bool bSupportsTransparency;

protected:
	TSharedPtr<class SBrowserCore> MarsUI2;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	void HandleOnUrlChanged(const FText& Text);
private:

	FString PrefixProcess(FString NewURL);

};
