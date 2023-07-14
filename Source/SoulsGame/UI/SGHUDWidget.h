#pragma once

#include "Blueprint/UserWidget.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/Level/SGInteractableActor.h"
#include "SoulsGame/Subsystems/SGTableRowData.h"
#include "SGHUDWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterSet, ASGCharacterBase*, Character);

UCLASS(Abstract)
class SOULSGAME_API USGHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	USGHUDWidget(const FObjectInitializer& ObjectInitializer);
	
public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void InitializeHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateHUDStatsFromCpp();

	void SetShouldShowStats(bool Set);

	UFUNCTION(BlueprintCallable)
	float GetCurrentDifficulty();
	
	UPROPERTY(BlueprintReadOnly)
	bool bShouldShowStats = false;

	void SetBossActor(ASGCharacterBase* Actor);
	UFUNCTION(BlueprintImplementableEvent)
	void SetBossActor_BP(ASGCharacterBase * Actor);

	void UnsetBossActor();
	UFUNCTION(BlueprintImplementableEvent)
	void UnsetBossActor_BP();

	void SetBossHealthPercent(float Percentage);
	UFUNCTION(BlueprintImplementableEvent)
	void SetBossHealthPercent_BP(float Percentage);

	UFUNCTION(BlueprintCallable)
	void UpdateBossHealth();

	void SetHealthPots(int32 Amount);
	UFUNCTION(BlueprintImplementableEvent)
	void SetHealthPots_BP(int32 Amount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisplayNarrative(const FSGNarrativeTableData& Data);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RemoveNarrative();
	
	void AddInteract(USGInteractEntryData* Data);
	void RemoveInteract(AActor* Owner);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddInteract(USGInteractEntryData* Data);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRemoveInteract(AActor* Owner);

	UFUNCTION(BlueprintCallable)
	void RequestUpdateAbilityAmounts();
	
	void UpdateAbilityAmounts(ASGCharacterBase* OwnerCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAbilityAmounts_BP(ASGCharacterBase* OwnerCharacter, int32 AmountAbilityOne, int32 AmountAbilityTwo);

	UPROPERTY(BlueprintAssignable)
	FOnBossCharacterSet OnBossCharacterSet;
};
