#pragma once

#include "Blueprint/UserWidget.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Level/InteractableActor.h"
#include "SoulsGame/Subsystems/SGTableRowData.h"
#include "HUDWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterSet, ASCharacterBase*, Character);

UCLASS(Abstract)
class SOULSGAME_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	UHUDWidget(const FObjectInitializer& ObjectInitializer);
	
public:

	void InitializeHUD(class UMyAttributeSet* AttributeSet);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateHUDStatsFromCpp();

	void SetShouldShowStats(bool Set);

	UFUNCTION(BlueprintCallable)
	float GetCurrentDifficulty();
	
	UPROPERTY(Transient, BlueprintReadOnly)
	class UMyAttributeSet* PlayerAttributeSet = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldShowStats = false;

	void SetBossActor(ASCharacterBase* Actor);
	UFUNCTION(BlueprintImplementableEvent)
	void SetBossActor_BP(ASCharacterBase * Actor);

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
	
	void AddInteract(UInteractEntryData* Data);
	void RemoveInteract(AActor* Owner);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddInteract(UInteractEntryData* Data);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRemoveInteract(AActor* Owner);

	UFUNCTION(BlueprintCallable)
	void RequestUpdateAbilityAmounts();
	
	void UpdateAbilityAmounts(ASCharacterBase* OwnerCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAbilityAmounts_BP(ASCharacterBase* OwnerCharacter, int32 AmountAbilityOne, int32 AmountAbilityTwo);

	UPROPERTY(BlueprintAssignable)
	FOnBossCharacterSet OnBossCharacterSet;
};
