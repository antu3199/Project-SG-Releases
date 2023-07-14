#pragma once
#include "GameplayTagContainer.h"
#include "SGSaveRecord.generated.h"

// Runtime data, not intended to be saved
USTRUCT()
struct FSGRuntimeData
{
	GENERATED_BODY()

	FGameplayTag SpawnPointTag;

	bool bHasLoadedFirstTime = false;

	bool bHasRequestedLoadOnMapStart = true;

	bool bOpenShopOnLoad = false;
};

USTRUCT(BlueprintType)
struct SOULSGAME_API FSGInventoryItemRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct SOULSGAME_API FSGInventoryItemRelicRecord : public FSGInventoryItemRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 RelicLevel = 1;
};


USTRUCT(BlueprintType)
struct SOULSGAME_API FSGInventoryRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FSGInventoryItemRecord> Items;
};

USTRUCT(BlueprintType)
struct SOULSGAME_API FSGSaveRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Version = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 MagicNumber = 123;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer UnlockedSavePoints;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag LastSavepointTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer LevelProgressionData;

	UPROPERTY(BlueprintReadOnly)
	FSGInventoryRecord InventoryRecord;

	FSGRuntimeData RuntimeData;
};

UINTERFACE(MinimalAPI)
class USGSaveLoadInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISGSaveLoadInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual void OnSaveGame(FSGSaveRecord& SaveRecord) = 0;
	virtual void LoadGame(const FSGSaveRecord& LoadRecord) = 0;
};
