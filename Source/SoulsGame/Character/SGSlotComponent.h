#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SGSlotComponent.generated.h"

#define SG_NUM_SLOTS 4

USTRUCT(BlueprintType)
struct FSGSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	bool bInUse = false;
	
	struct FSGInventoryItem* InventoryItem = nullptr;
	struct FSGAbilitySpec* AbilitySpec = nullptr;;
};

UCLASS()
class SOULSGAME_API USGSlotComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USGSlotComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AssignSlotSpec(int32 SlotIndex, const FGameplayTag& InventoryItemTag);

	UFUNCTION(BlueprintCallable)
	FSGSlot GetSlot(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	void UnAssignSlot(int32 SlotIndex);
	
	FSGSlot* GetSlotPtr(int32 Index);

	UFUNCTION(BlueprintCallable)
	int32 GetSlotIndexByItemTag(const FGameplayTag& InventoryItemTag) const;

	UFUNCTION(BlueprintCallable)
	void UseAbilityOnSlot(int32 Index) const;
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentCooldownTimeOnSlot(int32 Index) const;

	// Useful for UI
	UFUNCTION(BlueprintCallable)
	float GetCurrentCooldownPercentOnSlot(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	TArray<FSGSlot> GetAllSlots() const;
	
private:
	void PostBeginPlay();
	
	UPROPERTY()
	TArray<FSGSlot> Slots;
};
