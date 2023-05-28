#pragma once

#include "Blueprint/UserWidget.h"
#include "SoulsGame/Upgrades/SGUpgradeController.h"
#include "UpgradeMenuWidget.generated.h"




UCLASS(Abstract)
class SOULSGAME_API UUpgradeMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	UUpgradeMenuWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UpgradeMenu")
	void InitializeFromCpp(const TArray<FUpgradeOption>& UpgradeOptions);

	UFUNCTION(BlueprintCallable, Category = "UpgradeMenu")
	void SelectOptionToCpp(const FUpgradeOption& Option);

	UFUNCTION(BlueprintCallable, Category = "UpgradeMenu")
	void WidgetCloseToCpp();
};
