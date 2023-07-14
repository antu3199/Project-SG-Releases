#pragma once

#include "Blueprint/UserWidget.h"
#include "SGActorUIWidget.generated.h"


UCLASS(Abstract)
class SOULSGAME_API USGActorUIWidget : public UUserWidget
{
	GENERATED_BODY()
	USGActorUIWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	void CustomInitialize(class UWidgetComponent * InComponent, float InCurValue, float InMaxValue);

	UFUNCTION(BlueprintImplementableEvent)
	void CustomInitialize_BP(float Percentage);

	void SetValue(float InCurValue);
	UFUNCTION(BlueprintImplementableEvent)
	void SetValue_BP(float Percentage);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	
private:

	void SetDrawSize() const;
	void ShowUIWidget();
	void HideUIWidget();
	
	float MaxValue;
	float CurValue;

	TWeakObjectPtr<UWidgetComponent> Component;
	
	UPROPERTY(EditAnywhere, Category= "UI")
	float TimeDisplayed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	float UIDistanceScale = 350.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	bool bIsDisabled = false;

	
	bool bIsDisplayed = false;
	FVector2D OriginalActorUIDrawSize;

	FTimerHandle TimerHandle;
	
};
