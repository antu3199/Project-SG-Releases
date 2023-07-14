// Fill out your copyright notice in the Description page of Project Settings.


#include "SGGameModeBase.h"

#include "BrainComponent.h"
#include "EngineUtils.h"
#include "SGGameInstance.h"
#include "SGGameSingleton.h"
#include "SGSimpleLevelManagerSubsystem.h"
#include "StaticSGData.h"

#include "SoulsGame/Character/SGCharacterBase.h"
#include "SGUtils.h"

#include "Character/SGPlayerController.h"
#include "AI/SGEnemyAIController.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Level/SavePointActor.h"
#include "SaveLoad/SGSaveLoadManager.h"
#include "Subsystems/SGUISubsystem.h"
#include "UE4Overrides/SGCharacterMovementComponent.h"



void ASGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GameInstance->OnMapBeginPlay(this);
	}



	
	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
	}
	
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ASGGameModeBase::SpawnActorFromSpawners, SpawnRate,  true);
	GetWorld()->GetTimerManager().SetTimer(DifficultyIncreaseTimerHandle, this, &ASGGameModeBase::IncreaseDifficulty, DifficultyIncreaseSeconds,  true);

	GetWorld()->OnWorldBeginPlay.AddUObject(this, & ASGGameModeBase::PostBeginPlay);
	PlayerController = Cast<ASGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->InitializeUIForPlayer(PlayerController);
		Subsystem->AddHUDToViewport();
		Subsystem->InitializeFadeInOut(1.0f);

		Subsystem->UnFocusWidget();
	}


	/*
	if (USimpleLevelManagerSubsystem * LevelSubsystem = FSUtils::GetGameInstanceSubsystem<USimpleLevelManagerSubsystem>(GetWorld()))
	{
		LevelSubsystem->OnAllLevelsLoaded.AddDynamic(this, &AMyGameModeBase::OnAllLevelsLoaded);
		LevelSubsystem->LoadAllStreamingLevels();
	}
*/

	if (const USGSaveLoadManager* SaveLoadManager = USGSaveLoadManager::GetSingleton(GetWorld()))
	{
		m_SavepointTag = SaveLoadManager->GetCurrentSaveRecord().RuntimeData.SpawnPointTag;
#if WITH_EDITOR
		if (!SaveLoadManager->GetCurrentSaveRecord().RuntimeData.bHasLoadedFirstTime)
		{
			if (Debug_SpawnPoint.IsValid())
			{
				m_SavepointTag = Debug_SpawnPoint;
			}
		}
#endif
	}

	// TODO: For simplicity, just load all levels. Revisit if this becomes an issue later
	const TArray<ULevelStreaming*>& Levels = GetWorld()->GetStreamingLevels();
	m_NumStreamingLevels = Levels.Num();
	m_CurrentlyLoadedLevels = 0;

	if (USGSimpleLevelManagerSubsystem * LevelSubsystem = FSGUtils::GetGameInstanceSubsystem<USGSimpleLevelManagerSubsystem>(GetWorld()))
	{
		LevelSubsystem->OnLoadStreamingLevelFinished.AddDynamic(this, &ASGGameModeBase::OnStreamingLevelLoaded);
	}

	for (int32 i = 0; i < Levels.Num(); i++)
	{
		const ULevelStreaming* LevelToLoad = Levels[i];
		const FName LevelName =  LevelToLoad->GetWorldAssetPackageFName();
		if (USGSimpleLevelManagerSubsystem * LevelSubsystem = FSGUtils::GetGameInstanceSubsystem<USGSimpleLevelManagerSubsystem>(GetWorld()))
		{
			LevelSubsystem->LoadStreamingLevel(LevelName, true);
		}
	}
	

	// Loads only the savepoint level. Disabled for now for simplicity

	/*
	
	if (const USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		if (const UStaticSGData* StaticSGData = GameInstance->GetStaticData())
		{
			int32 Index = -1;
			for (int32 i = 0; i < StaticSGData->SavePointData.Num(); i++)
			{
				if (StaticSGData->SavePointData[i].Tag == m_SavepointTag)
				{
					Index = i;
					break;
				}
				
			}

			if (Index == -1)
			{
				UE_LOG(LogTemp, Warning, TEXT("Unable to find savepoint!"));
				return;
			}

			const FSavePointData& SavePointData = StaticSGData->SavePointData[Index];
			
			if (USimpleLevelManagerSubsystem * LevelSubsystem = FSUtils::GetGameInstanceSubsystem<USimpleLevelManagerSubsystem>(GetWorld()))
			{
				FName PathName = SavePointData.TransformData.PathName;
				LevelSubsystem->OnLoadStreamingLevelFinished.AddDynamic(this, &AMyGameModeBase::OnStreamingLevelLoaded);
				LevelSubsystem->LoadStreamingLevel(PathName, true);
			}
		}
	}
	*/
	
	
}

void ASGGameModeBase::BeginDestroy()
{
	if (SpawnTimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		}
	}

	if (DifficultyIncreaseTimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(DifficultyIncreaseTimerHandle);
		}
	}


	
	Super::BeginDestroy();
}

void ASGGameModeBase::PostInitProperties()
{
	Super::PostInitProperties();
}

void ASGGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	APawn* NewPlayerPawn = NewPlayer->GetPawn();
}

void ASGGameModeBase::RestartGame(bool bFadeIn)
{
	// Reference: AGameMode::RestartGame()
	if (GameSession->CanRestartGame() )
	{
		if (bFadeIn)
		{
			if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
			{
				Subsystem->OnUIFadeInCompleted.AddDynamic(this, &ASGGameModeBase::ASGGameModeBase::OnFadeInCompleted_RestartLevel);
				Subsystem->InitializeFadeInOut(0.0f);
				Subsystem->DoFadeIn();
			}
		}
		else
		{
			GetWorld()->ServerTravel("?Restart",false);
		}
	}
}

void ASGGameModeBase::RestartGameAtSavePoint(const FGameplayTag& SavePointTag, bool bOpenMenuOnLoad)
{
	if (USGSaveLoadManager* SaveLoadManager = USGSaveLoadManager::GetSingleton(GetWorld()))
	{
		FSGRuntimeData& RuntimeData = SaveLoadManager->GetCurrentSaveRecordMutable().RuntimeData;
		RuntimeData.SpawnPointTag = SavePointTag;
		RuntimeData.bOpenShopOnLoad = bOpenMenuOnLoad;
	}

	RestartGame();
}

void ASGGameModeBase::PostBeginPlay()
{

	
	GetWorld()->OnWorldBeginPlay.RemoveAll(this);



	

}

void ASGGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// if (TestSavepoint)
	// {
	// 	TestSavepoint->OpenSaveUI(GetPlayerController());
	// }
}

void ASGGameModeBase::RegisterSpawner(ASGSpawnerActor* Spawner)
{
	SpawnerActors.Add(Spawner);
}

void ASGGameModeBase::RemoveSpawner(ASGSpawnerActor* SpawnerActor)
{
	if (SpawnerActors.Contains(SpawnerActor))
	{
		SpawnerActors.Remove(SpawnerActor);
	}
}

void ASGGameModeBase::SpawnActorFromSpawners()
{
	if (SpawnerActors.Num() == 0)
	{
		return;
	}

	if (bDisableSpawning == true)
	{
		return;
	}

	const int32 Index = FMath::RandRange(0, SpawnerActors.Num() - 1);
	ASGSpawnerActor* SpawnerActor = SpawnerActors[Index];
	AActor* NewlySpawnedActor = SpawnerActor->SpawnRandomActorFromTemplate();

	// DEPRECATED
	/*
	if (ASCharacterBase* NewlySpawnedCharacter =  Cast<ASCharacterBase>(NewlySpawnedActor))
	{
		UMyAttributeSet* AttributeSet = NewlySpawnedCharacter->GetAttributeSet();
		AttributeSet->MultiplyStats(CurrentDifficulty);
	}
	*/
}

void ASGGameModeBase::IncreaseDifficulty()
{
	CurrentDifficulty += DifficultyIncreaseAmount;
}

float ASGGameModeBase::GetCurrentDifficulty() const
{
	return CurrentDifficulty;
}

IInterface_PostProcessVolume* ASGGameModeBase::GetGlobalPostProcessVolume() const
{
	if (UWorld* World = GetWorld())
	{
		for (const auto& Volume : World->PostProcessVolumes)
		{
			FPostProcessVolumeProperties Properties = Volume->GetProperties();
			if (Properties.bIsUnbound)
			{
				return Volume;
			}
		}
	}

	return nullptr;
}

void ASGGameModeBase::AddTimestopMaterial()
{

	FPostProcessSettings* Settings = (FPostProcessSettings*)GetGlobalPostProcessVolume()->GetProperties().Settings;
	// Check for existence before add
	for (FWeightedBlendable& Blendable : Settings->WeightedBlendables.Array)
	{
		if (Blendable.Object == TimestopMaterialObject)
		{
			return;
		}
	}

	FSGUtils::bIsTimestopped = true;

	FWeightedBlendable NewBlendable;
	NewBlendable.Weight = 1.0f;
	NewBlendable.Object = TimestopMaterialObject;
	
	Settings->WeightedBlendables.Array.Add(NewBlendable);
}

void ASGGameModeBase::RemoveTimestopMaterial()
{
	FPostProcessSettings* Settings = (FPostProcessSettings*)GetGlobalPostProcessVolume()->GetProperties().Settings;

	
	int32 DeletionIndex = -1;
	// Check for existence before add
	for (int32 i = 0; i < Settings->WeightedBlendables.Array.Num(); i++)
	{
		if (Settings->WeightedBlendables.Array[i].Object == TimestopMaterialObject)
		{
			DeletionIndex = i;
			break;
		}
	}

	if (DeletionIndex != -1)
	{
		Settings->WeightedBlendables.Array.RemoveAt(DeletionIndex);
		FSGUtils::bIsTimestopped = false;
	}
}

void ASGGameModeBase::TogglePlayerAutoBattle()
{
	ASGCharacterBase* Character = GetCharacter();
	if (bIsUsingAutoBattle)
	{
		//Character->GetCharacterMovementComponent()->bOrientRotationToMovement = true;
		Character->bUseControllerRotationYaw = false;
		// Disable auto battle
		if (PlayerAIController != nullptr)
		{
			PlayerAIController->BrainComponent->StopLogic(TEXT("ToggleAutoBattle"));
		}

		PlayerController->Possess(Character);
	}
	else
	{
		if (IsValid(PlayerAIController))
		{
			PlayerAIController->BrainComponent->RestartLogic();
		}
		else
		{
			PlayerAIController = GetWorld()->SpawnActor<ASGEnemyAIController>(PlayerAIControllerTemplate);
			SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass);
		}

		
		//Character->GetCharacterMovementComponent()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayerAIController->Possess(Character);
	}

	bIsUsingAutoBattle = !bIsUsingAutoBattle;
}

ASGPlayerController* ASGGameModeBase::GetPlayerController() const
{
	return PlayerController;
}

ASGEnemyAIController* ASGGameModeBase::GetPlayerAIController() const
{
	return PlayerAIController;
}

ASGCharacterBase* ASGGameModeBase::GetCharacter() const
{
	return Cast<ASGCharacterBase>(PlayerController->GetCurrentPawnBasedOnIndex());
}

void ASGGameModeBase::HandlePlayerStart(FVector Location, AActor* Player)
{
	FSGUtils::SetSpringArmLagOnActor(Player, false);
		
	Player->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
	{
		FSGUtils::SetSpringArmLagOnActor(Player, true);
	});
		
	//GetWorld()->UpdateLevelStreaming();
	GetWorld()->WorldComposition->UpdateStreamingState(Location);
	GetWorld()->FlushLevelStreaming();
}

void ASGGameModeBase::OnAllLevelsLoaded()
{

	bool bFoundPlacement = false;
#if WITH_EDITOR
	// Allow for "Play from here"
	if (USGGameSingleton* Singleton = Cast<USGGameSingleton>(GEngine->GameSingleton))
	{
		if (Singleton->bHasWorldPlacement && Singleton->EditorWorldName == GetWorld()->GetName())
		{
			bFoundPlacement = true;
		}
	}
#endif


	if (!bFoundPlacement)
	{
		ASavePointActor* SavePointToUse = nullptr;
		if (APawn* PlayerPawn = GetPlayerController()->GetPawnCharacter())
		{
			if (const USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
	
				/*
				for (TObjectIterator<ASavePointActor> It; It; ++It)
				{
					ASavePointActor* Savepoint = *It;
	
					FVector TestLocation = Savepoint->GetActorLocation();
					UE_LOG(LogTemp, Display, TEXT("Test location: %s %s"), *Savepoint->GetActorLocation().ToString(), *Savepoint->SavePointTag.ToString());
					
					
					if (Savepoint->SavePointTag == m_SavepointTag)
					{
						SavePointToUse = Savepoint;
						break;
					}
				}
				*/
	
				for (TActorIterator<ASavePointActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
				{
					if (ActorItr->SavePointTag == m_SavepointTag)
					{
						SavePointToUse = *ActorItr;
					}
				}
	
				/*
				TArray<ASavePointActor*> AllSavepointsInTheGame;
				const TArray<ULevelStreaming*>& Levels = GetWorld()->GetStreamingLevels();
				TSet<AActor*> ActorSet;
	
				for (int32 i = 0; i < Levels.Num(); i++)
				{
					ULevel* Level = Levels[i]->GetLoadedLevel();
					if (Level != nullptr)
					{
						for (AActor* Actor : Level->Actors)
						{
							if (ASavePointActor* CastedActor = Cast<ASavePointActor>(Actor))
							{
								AllSavepointsInTheGame.Add(CastedActor);
							}
						}
					}
				}
	
				ASavePointActor** FoundActor = AllSavepointsInTheGame.FindByPredicate([=](const ASavePointActor* Actor)
				{
					return Actor->SavePointTag == m_SavepointTag;
				});
	
				if (!FoundActor)
				{
					check(false);
					UE_LOG(LogTemp, Error, TEXT("Cannot find save point!"));
					return;
				}
	
				SavePointToUse = *FoundActor;
				*/
	
				if (SavePointToUse)
				{
					const FVector Location = SavePointToUse->GetSpawnPoint();
					FRotator Rotation = SavePointToUse->GetSpawnRotation();
	
					// Note: Spring arm lag will be reset in USGUISubsystem::OnRemoveFade
					FSGUtils::SetSpringArmLagOnActor(PlayerPawn, false);
					PlayerPawn->SetActorLocationAndRotation(Location, Rotation);
					bFoundPlacement = true;
				}
			}
	
			if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				if (USGSaveLoadManager* SaveLoadManager = USGSaveLoadManager::GetSingleton(GetWorld()))
				{
					SaveLoadManager->GetCurrentSaveRecordMutable().RuntimeData.bHasLoadedFirstTime = true;
		
					// Set progression for Debugging purposes
					if (SavePointToUse != nullptr)
					{
						if (!SavePointToUse->VisiblityRequirements.IsEmpty())
						{
							TArray<FGameplayTag> VisibilityTags;
							SavePointToUse->VisiblityRequirements.GetGameplayTagArray(VisibilityTags);
							for (const FGameplayTag& VisibilityTag : VisibilityTags)
							{
								GameInstance->SetLevelProgression(VisibilityTag);
								UE_LOG(LogTemp, Display, TEXT("Debug visibility requirements set: %s"), *VisibilityTag.ToString());
							}
						}
		
						// Open shop on load
						if (SaveLoadManager->GetCurrentSaveRecord().RuntimeData.bOpenShopOnLoad)
						{
							ASGPlayerController* Controller = GetPlayerController();
							SavePointToUse->OpenSaveUI(Controller);
						}
					}

					SaveLoadManager->GetCurrentSaveRecordMutable().RuntimeData.bOpenShopOnLoad = false;
				}
			}
		}
	}


	OnSGAllLevelsLoaded.Broadcast();

	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnUIFadeInCompleted.AddDynamic(this, &ASGGameModeBase::OnFadeOutCompleted_Initialize);
		Subsystem->DoFadeOut();
	}
}

void ASGGameModeBase::OnStreamingLevelLoaded(FName LevelLoaded)
{
	m_CurrentlyLoadedLevels++;
	if (m_CurrentlyLoadedLevels == m_NumStreamingLevels)
	{
		OnAllLevelsLoaded();
	}
}

void ASGGameModeBase::OnFadeOutCompleted_Initialize()
{
	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnUIFadeInCompleted.RemoveDynamic(this, &ASGGameModeBase::OnFadeOutCompleted_Initialize);
	}

	// Hack to stop camera spring from messing up on teleport
	if (PlayerController)
	{
		if (APawn* Pawn = PlayerController->GetPawn())
		{
			FSGUtils::SetSpringArmLagOnActor(Pawn, true);
		}
	}
}

void ASGGameModeBase::OnFadeInCompleted_RestartLevel()
{
	RestartGame(false);
}
