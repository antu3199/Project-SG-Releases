#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"


UCLASS(Abstract)
class SOULSGAME_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	UFUNCTION(BlueprintCallable)
	void CustomInitialize();

	UFUNCTION(BlueprintCallable)
	void OnGameStartPressed();
	
	UFUNCTION(BlueprintCallable)
	void OnSettingsButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnExitGamePressed();
};
