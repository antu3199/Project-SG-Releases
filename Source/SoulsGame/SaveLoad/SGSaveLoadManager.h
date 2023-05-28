#pragma once
#include "CoreMinimal.h"
#include "SGSaveRecord.h"


#include "SGSaveLoadManager.generated.h"

#define DEFAULT_SAVE_FILE "SGSaveFile.json"

DECLARE_LOG_CATEGORY_EXTERN(SGSaveLoadManager, Log, All);

// TODO: Separate subsystem
UCLASS(BlueprintType)
class SOULSGAME_API USGSaveLoadManager : public UObject 
{
public:
	GENERATED_BODY()
	static USGSaveLoadManager* GetSingleton(const UObject* WorldContextObject);

	// Note: Save/Load game must be done OnPostBeginPlay if first level is the game because it loads actor information
	void SaveGame();
	void SaveGame(const struct FSGSaveRecord& SaveRecord, const FString& SavePath) const;
	void LoadGameFromFile();

	void LoadGame(const struct FSGSaveRecord& SaveRecord);

	void LoadEmptyGame();
	void ClearSaveData();

	UFUNCTION(BlueprintCallable)
	const FSGSaveRecord& GetCurrentSaveRecord() const;

	UFUNCTION(BlueprintCallable)
	FSGSaveRecord& GetCurrentSaveRecordMutable();

private:

	FSGSaveRecord CurrentSaveRecord;
	
	FString GetFullSavePath() const;

	void GetAllActorsFromAllStreamingLevels(TArray<ISGSaveLoadInterface*>& OutActors) const;
	
};

