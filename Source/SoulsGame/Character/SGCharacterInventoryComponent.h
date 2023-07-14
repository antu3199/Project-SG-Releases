#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

// TODO: Maybe not needed here

#include "SoulsGame/StaticSGData.h"
#include "SGCharacterInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FSGInventoryItem
{
	GENERATED_BODY()

	friend class USGCharacterInventoryComponent;

	static struct FSGStaticRelicItem* GetStaticRelicDataFromTag(const UObject* WorldContextObject, const FGameplayTag& Tag);

	FGameplayTag GetTag() const;

	FSGStaticInventoryItem* GetStaticData() const { return StaticData; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity = 1;

protected:
	FGameplayTag GetTagFromStaticData() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tag;
	
	FSGStaticInventoryItem* StaticData = nullptr;
};


UCLASS()
class SOULSGAME_API USGCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USGCharacterInventoryComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int32 MaxHealthPots = 3;

	UFUNCTION(BlueprintCallable)
	void SetCurHealthPots(int32 Pots);

	UFUNCTION(BlueprintCallable)
	int32 GetCurHealthPots()
	{
		return CurHealthPots;
	}

	UFUNCTION(BlueprintCallable)
	void ResetAbilityUses();

	UFUNCTION(BlueprintCallable)
	void AddItem(UPARAM(meta = (Categories = "Items")) FGameplayTag ItemTag, int32 Quantity);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(UPARAM(meta = (Categories = "Items")) FGameplayTag ItemTag, int32 Quantity);
	
	UFUNCTION(BlueprintCallable)
	int32 GetNumItems(const FSGInventoryItem& Item);

	// TODO: Maybe move this to a different subsystem?
	UFUNCTION(BlueprintCallable)
	FSGStaticInventoryItem GetStaticData(const FSGInventoryItem& Item, bool& OutResult) const;

	UFUNCTION(BlueprintCallable)
	FSGStaticInventoryItem GetStaticDataByTag(const FGameplayTag& ItemTag, bool& OutResult);

	UFUNCTION(BlueprintCallable)
	FSGStaticRelicItem GetStaticRelicDataByTag(UPARAM(meta = (Categories = "Items")) FGameplayTag ItemTag, bool& OutResult);
	
	UFUNCTION(BlueprintCallable)
	FSGStaticRelicItem GetStaticRelicDataByAbilityTag(UPARAM(meta = (Categories = "Ability")) FGameplayTag ItemTag, bool& OutResult);
	
	FSGInventoryItem* GetItemPtr(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	TArray<FSGInventoryItem> GetItemsMatchingTag(UPARAM(meta = (Categories = "Items")) FGameplayTag Tag);
	
	FSGInventoryItem GetItemWithTag(UPARAM(meta = (Categories = "Items")) FGameplayTag Tag);
	
	UPROPERTY(EditAnywhere)
	int32 InitialAbilityOneUses = 20;

	UPROPERTY(EditAnywhere)
	int32 InitialAbilityTwoUses = 5;

	// TODO change after ability system refactor
	UPROPERTY(BlueprintReadWrite)
	int32 TEMP_AbilityOneUses = -1;

	UPROPERTY(BlueprintReadWrite)
	int32 TEMP_AbilityTwoUses = -1;

	TArray<FSGInventoryItem> Items;
	
private:
	int32 CurHealthPots;

	class USGHUDWidget * GetHUD() const;
};
