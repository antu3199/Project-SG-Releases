#pragma once
#include "GameplayTagContainer.h"
#include "SGSaveRecord.generated.h"

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
	virtual void SaveGame(FSGSaveRecord& SaveRecord) = 0;
	virtual void LoadGame(const FSGSaveRecord& LoadRecord) = 0;
};
