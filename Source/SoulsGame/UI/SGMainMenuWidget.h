#pragma once

#include "Blueprint/UserWidget.h"
#include "SGMainMenuWidget.generated.h"


UCLASS(Abstract)
class SOULSGAME_API USGMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	USGMainMenuWidget(const FObjectInitializer& ObjectInitializer);
	
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
