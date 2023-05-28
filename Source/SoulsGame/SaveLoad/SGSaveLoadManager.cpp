#include "SGSaveLoadManager.h"

#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGameInstance.h"

#include "SGSaveRecord.h"
#include "Engine/LevelStreaming.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Character/SCharacterBase.h"

DEFINE_LOG_CATEGORY(SGSaveLoadManager);

USGSaveLoadManager* USGSaveLoadManager::GetSingleton(const UObject* WorldContextObject)
{
	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		return GameInstance->GetSaveLoadManager();
	}

	return nullptr;
}

void USGSaveLoadManager::SaveGame()
{
	const FString SavePath = GetFullSavePath();

	// Save player
	/*
	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		CharacterBase->SaveGame(CurrentSaveRecord);
	}
	*/

	

	TArray<ISGSaveLoadInterface*> OutActors;
	GetAllActorsFromAllStreamingLevels(OutActors);

	for (ISGSaveLoadInterface* Actor : OutActors)
	{
		Actor->SaveGame(CurrentSaveRecord);
	}

	SaveGame(CurrentSaveRecord, SavePath);
}

void USGSaveLoadManager::SaveGame(const FSGSaveRecord& SaveRecord, const FString& SavePath) const
{
	FString JsonString;
	if (FJsonObjectConverter::UStructToJsonObjectString(SaveRecord, JsonString))
	{
		if (FFileHelper::SaveStringToFile(JsonString, *SavePath, FFileHelper::EEncodingOptions::ForceUTF8))
		{
			UE_LOG(SGSaveLoadManager, Verbose, TEXT("Saved to %s."), *SavePath);
		}
		else
		{
			UE_LOG(SGSaveLoadManager, Error, TEXT("Failed to save to path %s."), *SavePath);
		}
	}
}

void USGSaveLoadManager::LoadGameFromFile()
{
	const FString SavePath = GetFullSavePath();

	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *SavePath) || JsonString.IsEmpty())
	{
		UE_LOG(SGSaveLoadManager, Verbose, TEXT("No save game data found for slot %s."), *SavePath);
		return;
	}

	FSGSaveRecord SaveRecord;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &SaveRecord))
	{
		UE_LOG(SGSaveLoadManager, Verbose, TEXT("Unable to read struct data from %s."), *SavePath);
	}
	
	LoadGame(SaveRecord);
}

void USGSaveLoadManager::LoadGame(const FSGSaveRecord& SaveRecord)
{
	// Player
	/*
	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		CharacterBase->LoadGame(SaveRecord);
	}
	*/

	TArray<ISGSaveLoadInterface*> OutActors;
	GetAllActorsFromAllStreamingLevels(OutActors);

	for (ISGSaveLoadInterface* Actor : OutActors)
	{
		Actor->LoadGame(CurrentSaveRecord);
	}
	
	CurrentSaveRecord = SaveRecord;
}

void USGSaveLoadManager::LoadEmptyGame()
{
	const FString SavePath = GetFullSavePath();
	LoadGame(FSGSaveRecord());
}

void USGSaveLoadManager::ClearSaveData()
{
	const FString SavePath = GetFullSavePath();
	SaveGame(FSGSaveRecord(), SavePath);
	LoadGameFromFile();
}

const FSGSaveRecord& USGSaveLoadManager::GetCurrentSaveRecord() const
{
	return CurrentSaveRecord;
}

FSGSaveRecord& USGSaveLoadManager::GetCurrentSaveRecordMutable()
{
	return CurrentSaveRecord;
}


FString USGSaveLoadManager::GetFullSavePath() const
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), DEFAULT_SAVE_FILE);
}

void USGSaveLoadManager::GetAllActorsFromAllStreamingLevels(TArray<ISGSaveLoadInterface*>& OutActors) const
{
	//FSUtils::LoadAllStreamingLevels(GetWorld());
	
	const TArray<ULevelStreaming*>& Levels = GetWorld()->GetStreamingLevels();
	TSet<AActor*> ActorSet;

	for (int32 i = 0; i < Levels.Num(); i++)
	{
		ULevel* Level = Levels[i]->GetLoadedLevel();
		if (Level != nullptr)
		{
			for (AActor* Actor : Level->Actors)
			{
				if (Actor->GetClass()->ImplementsInterface(USGSaveLoadInterface::StaticClass()))
				{
					if (ISGSaveLoadInterface* ActorInterface = Cast<ISGSaveLoadInterface>(Actor))
					{
						OutActors.Add(ActorInterface);
					}
				}
			}
		}
	}
}
