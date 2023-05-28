#include "SGameInstance.h"

#include "EngineUtils.h"
#include "GameplayTagsManager.h"
#include "MyGameModeBase.h"
#include "SGameSingleton.h"
#include "SHitboxManager.h"
#include "StaticSGData.h"
#include "SUtils.h"
#include "Controllers/SGMarkerController.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/SGSaveLoadManager.h"
#include "Upgrades/SGUpgradeController.h"
bool USGameInstance::Debug_ShowHitboxes = false;

void USGameInstance::Init()
{
	Super::Init();
}

void USGameInstance::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UpgradeController = NewObject<USGUpgradeController>(this, PowerupControllerTemplate);
		UpgradeController->InitializePool();

		MarkerController = NewObject<USGMarkerController>(this, MarkerControllerTemplate);
		MarkerController->Initialize();

		SaveLoadManager = NewObject<USGSaveLoadManager>(this);
	}
}

void USGameInstance::OnStart()
{
	Super::OnStart();
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();
}


void USGameInstance::OnMapBeginPlay(AMyGameModeBase* GameModeBase)
{
	if (RuntimeData.bHasRequestedLoadOnMapStart)
	{
		SaveLoadManager->LoadGameFromFile();
		SetupRuntimeData();

		RuntimeData.bHasRequestedLoadOnMapStart = false;
	}
}

void USGameInstance::SetLevelProgression(FGameplayTag Tag)
{
	SaveLoadManager->GetCurrentSaveRecordMutable().LevelProgressionData.AddTag(Tag);
	OnLevelProgressionSet.Broadcast(Tag);
}


USGUpgradeController* USGameInstance::GetUpgradeController() const
{
	return UpgradeController;
}

USGMarkerController* USGameInstance::GetMarkerController() const
{
	return MarkerController;
}

#if WITH_EDITOR

FVector USGameInstance::GetEditorSpawnLocation() const
{
	return EditorStartLocation;
}


#endif

USGSaveLoadManager* USGameInstance::GetSaveLoadManager()
{
	return SaveLoadManager;
}

void USGameInstance::OnFirstStreamingLevelLoaded()
{
}

void USGameInstance::OnLevelLoadedTest()
{
	int32 Test = 0;
}

void USGameInstance::OnPostBeginPlayGame()
{


}

void USGameInstance::Debug_SaveGame()
{
	SaveLoadManager->SaveGame();
}

void USGameInstance::Debug_LoadGame()
{
	SaveLoadManager->LoadGameFromFile();
}

void USGameInstance::Debug_LoadEmptyGame()
{
	SaveLoadManager->LoadEmptyGame();
}

void USGameInstance::Debug_ClearSaveData()
{
	SaveLoadManager->ClearSaveData();
}

void USGameInstance::Debug_LoadAllStreamingLevels()
{
	//FSUtils::LoadAllStreamingLevels(GetWorld());
}

void USGameInstance::Debug_TestRestartLevelAtLocation()
{
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Progression.SavePoints.L1_2"));
	if (AMyGameModeBase* GameModeBase = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameModeBase->RestartGameAtSavePoint(Tag);
		//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

void USGameInstance::Debug_StreamingFunction(int32 Linkage)
{
	// Keep for now for educational purposes
}

// Setup runtime data, typically after loading
void USGameInstance::SetupRuntimeData()
{
	const FSGSaveRecord& SaveRecord = SaveLoadManager->GetCurrentSaveRecord();
	if (SaveRecord.LastSavepointTag.IsValid())
	{
		RuntimeData.SpawnPointTag = SaveRecord.LastSavepointTag;
	}
	else
	{
		// Use first tag as fallback
		RuntimeData.SpawnPointTag = StaticData->SavePointData[0].Tag;
	}
	
}

