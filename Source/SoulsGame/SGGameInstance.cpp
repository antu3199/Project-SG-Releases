#include "SGGameInstance.h"

#include "EngineUtils.h"
#include "GameplayTagsManager.h"
#include "SGGameModeBase.h"
#include "SGGameSingleton.h"
#include "StaticSGData.h"
#include "SGUtils.h"
#include "Controllers/SGMarkerController.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/SGSaveLoadManager.h"
bool USGGameInstance::Debug_ShowHitboxes = false;

void USGGameInstance::Init()
{
	Super::Init();
}

void USGGameInstance::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		MarkerController = NewObject<USGMarkerController>(this, MarkerControllerTemplate);
		MarkerController->Initialize();

		SaveLoadManager = NewObject<USGSaveLoadManager>(this);
	}
}

void USGGameInstance::OnStart()
{
	Super::OnStart();
}

void USGGameInstance::Shutdown()
{
	Super::Shutdown();
}


void USGGameInstance::OnMapBeginPlay(ASGGameModeBase* GameModeBase)
{
	if (SaveLoadManager->GetCurrentSaveRecord().RuntimeData.bHasRequestedLoadOnMapStart)
	{
		SaveLoadManager->LoadGameFromFile();
		SetupRuntimeData();

		SaveLoadManager->GetCurrentSaveRecordMutable().RuntimeData.bHasRequestedLoadOnMapStart = false;
	}
}

void USGGameInstance::SetLevelProgression(FGameplayTag Tag)
{
	SaveLoadManager->GetCurrentSaveRecordMutable().LevelProgressionData.AddTag(Tag);
	OnLevelProgressionSet.Broadcast(Tag);
}

USGMarkerController* USGGameInstance::GetMarkerController() const
{
	return MarkerController;
}

#if WITH_EDITOR

FVector USGGameInstance::GetEditorSpawnLocation() const
{
	return EditorStartLocation;
}


#endif

USGSaveLoadManager* USGGameInstance::GetSaveLoadManager()
{
	return SaveLoadManager;
}

void USGGameInstance::OnFirstStreamingLevelLoaded()
{
}

void USGGameInstance::OnLevelLoadedTest()
{
	int32 Test = 0;
}

void USGGameInstance::OnPostBeginPlayGame()
{


}

void USGGameInstance::Debug_SaveGame()
{
	SaveLoadManager->SaveGame();
}

void USGGameInstance::Debug_LoadGame()
{
	SaveLoadManager->LoadGameFromFile();
}

void USGGameInstance::Debug_LoadEmptyGame()
{
	SaveLoadManager->LoadEmptyGame();
}

void USGGameInstance::Debug_ClearSaveData()
{
	SaveLoadManager->ClearSaveData();
}

void USGGameInstance::Debug_LoadAllStreamingLevels()
{
	//FSUtils::LoadAllStreamingLevels(GetWorld());
}

void USGGameInstance::Debug_TestRestartLevelAtLocation()
{
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Progression.SavePoints.L1_2"));
	if (ASGGameModeBase* GameModeBase = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameModeBase->RestartGameAtSavePoint(Tag);
		//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

void USGGameInstance::Debug_StreamingFunction(int32 Linkage)
{
	// Keep for now for educational purposes
}

// Setup runtime data, typically after loading
void USGGameInstance::SetupRuntimeData()
{
	const FSGSaveRecord& SaveRecord = SaveLoadManager->GetCurrentSaveRecord();
	if (SaveRecord.LastSavepointTag.IsValid())
	{
		SaveLoadManager->GetCurrentSaveRecordMutable().RuntimeData.SpawnPointTag = SaveRecord.LastSavepointTag;
	}
	else
	{
		// Use first tag as fallback
		SaveLoadManager->GetCurrentSaveRecordMutable().RuntimeData.SpawnPointTag = StaticData->SavePointData[0].Tag;
	}
	
}

