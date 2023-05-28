#pragma once

#include "Blueprint/UserWidget.h"
#include "SGFadeInOutWidget.generated.h"

UCLASS(Abstract)
class SOULSGAME_API USGFadeInOutWidget : public UUserWidget
{
	GENERATED_BODY()
	USGFadeInOutWidget(const FObjectInitializer& ObjectInitializer);
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetFadeOpacity(float FadeOpacity);

	UFUNCTION(BlueprintImplementableEvent)
	void DoFadeIn();

	UFUNCTION(BlueprintImplementableEvent)
	void DoFadeOut();

	UFUNCTION(BlueprintImplementableEvent)
	void RequestCancel();
	
	UFUNCTION(BlueprintCallable)
	void OnFadeInFinished();

	UFUNCTION(BlueprintCallable)
	void OnFadeOutFinished();
};

