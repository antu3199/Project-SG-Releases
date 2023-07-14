#pragma once

#include "CoreMinimal.h"
#include "SGTableRowData.h"

#include "SGUISubsystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIFadeInCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIFadeOutCompleted);


UCLASS(config = Game, meta = (DisplayName = "SG UI Subsystem"), defaultconfig)
class USGUISubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	
public:
	UPROPERTY(Config, EditDefaultsOnly)
	TSubclassOf<class USGHUDWidget> HUDClass;

	UPROPERTY(Config, EditDefaultsOnly)
	TSubclassOf<class USGFadeInOutWidget> FadeInOutClass;
};

UCLASS()
class USGUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class USGFadeInOutWidget;

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintCallable)
	void InitializeUIForPlayer(APlayerController* InController, bool bInitializeGameUI = true);

	UFUNCTION(BlueprintCallable)
	UUserWidget* AddArbitraryUIToViewport(TSubclassOf<class UUserWidget> WidgetClass, bool bFocusWidget = true, bool bAddToList = true);

	UFUNCTION(BlueprintCallable)
	void RemoveArbitraryUIFromViewport(class UUserWidget* Widget, bool bUnFocusWidget = true, bool bRemoveFromList = false);
	
	UFUNCTION(BlueprintCallable)
	void AddHUDToViewport();

	UFUNCTION(BlueprintCallable)
	void RemoveHUDFromViewport();

	UFUNCTION(BlueprintCallable)
	void FocusWidget(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable)
	void UnFocusWidget();

	UFUNCTION(BlueprintCallable)
	class USGHUDWidget* GetHUDInstance() const { return HUDInstance; }

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetActiveWidgetByClass(TSubclassOf<class UUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
	class USGFadeInOutWidget* InitializeFadeInOut(float StartOpacity);

	UFUNCTION(BlueprintCallable)
	class USGFadeInOutWidget* GetFadeInOutWidgetInstance() const;

	UFUNCTION(BlueprintCallable)
	void DoFadeIn();

	UFUNCTION(BlueprintCallable)
	void DoFadeOut();

	UFUNCTION(BlueprintCallable)
	FString GetVersionString() const;

	UPROPERTY(BlueprintAssignable)
	FOnUIFadeInCompleted OnUIFadeInCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnUIFadeOutCompleted OnUIFadeOutCompleted;

private:
	void OnFadeIn();
	void OnFadeOut();

	bool bFadeInRequested = false;
	bool bFadeOutRequested = false;
	
	UPROPERTY(Transient)
	class USGHUDWidget *HUDInstance = nullptr;

	UPROPERTY(Transient)
	class USGFadeInOutWidget* ActiveFadeInOutWidget = nullptr;

	UPROPERTY()
	TSet<UUserWidget*> ActiveWidgets;

	TWeakObjectPtr<APlayerController> Controller = nullptr;
};
