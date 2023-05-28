#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "SavePointMenuWidget.generated.h"






UCLASS(Abstract)
class SOULSGAME_API USavePointMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	USavePointMenuWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	UFUNCTION(BlueprintCallable)
	void CustomInitialize();

	UFUNCTION(BlueprintCallable)
	void DoQuickTravel();

	UFUNCTION(BlueprintCallable)
	void DoSave();

	UFUNCTION(BlueprintCallable)
	void DoLoad();

	UFUNCTION(BlueprintCallable)
	void DoReturnToGame();

	UFUNCTION(BlueprintCallable)
	void ExitToMainMenu();

	UFUNCTION(BlueprintCallable)
	void Debug_UnlockAll();
	
	UFUNCTION(BlueprintCallable)
	TArray<class USavePointEntryData*> GetSavePointEntryData() const;
};
