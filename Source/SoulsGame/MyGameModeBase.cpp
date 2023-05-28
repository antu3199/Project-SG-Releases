// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

#include "BrainComponent.h"
#include "EngineUtils.h"
#include "SGameInstance.h"
#include "SGameSingleton.h"
#include "SimpleLevelManagerSubsystem.h"
#include "StaticSGData.h"

#include "SoulsGame/Character/SCharacterBase.h"
#include "SUtils.h"

#include "Character/MyPlayerController.h"
#include "Character/PlayerAIController.h"
#include "Engine/LevelStreaming.h"
#include "Engine/WorldComposition.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Level/SavePointActor.h"
#include "SaveLoad/SGSaveLoadManager.h"
#include "Subsystems/UISubsystem.h"
#include "UE4Overrides/MyCharacterMovementComponent.h"



void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GameInstance->OnMapBeginPlay(this);
	}



	
	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
	}
	
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AMyGameModeBase::SpawnActorFromSpawners, SpawnRate,  true);
	GetWorld()->GetTimerManager().SetTimer(DifficultyIncreaseTimerHandle, this, &AMyGameModeBase::IncreaseDifficulty, DifficultyIncreaseSeconds,  true);

	GetWorld()->OnWorldBeginPlay.AddUObject(this, & AMyGameModeBase::PostBeginPlay);
	PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
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

	if (const USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		m_SavepointTag = GameInstance->GetRuntimeData().SpawnPointTag;
#if WITH_EDITOR

		if (!GameInstance->GetRuntimeData().bHasLoadedFirstTime)
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

	if (USimpleLevelManagerSubsystem * LevelSubsystem = FSUtils::GetGameInstanceSubsystem<USimpleLevelManagerSubsystem>(GetWorld()))
	{
		LevelSubsystem->OnLoadStreamingLevelFinished.AddDynamic(this, &AMyGameModeBase::OnStreamingLevelLoaded);
	}

	for (int32 i = 0; i < Levels.Num(); i++)
	{
		const ULevelStreaming* LevelToLoad = Levels[i];
		const FName LevelName =  LevelToLoad->GetWorldAssetPackageFName();
		if (USimpleLevelManagerSubsystem * LevelSubsystem = FSUtils::GetGameInstanceSubsystem<USimpleLevelManagerSubsystem>(GetWorld()))
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

void AMyGameModeBase::BeginDestroy()
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

void AMyGameModeBase::PostInitProperties()
{
	Super::PostInitProperties();
}

void AMyGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	APawn* NewPlayerPawn = NewPlayer->GetPawn();
}

void AMyGameModeBase::RestartGame(bool bFadeIn)
{
	// Reference: AGameMode::RestartGame()
	if (GameSession->CanRestartGame() )
	{
		if (bFadeIn)
		{
			if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
			{
				Subsystem->OnUIFadeInCompleted.AddDynamic(this, &AMyGameModeBase::AMyGameModeBase::OnFadeInCompleted_RestartLevel);
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

void AMyGameModeBase::RestartGameAtSavePoint(const FGameplayTag& SavePointTag, bool bOpenMenuOnLoad)
{
	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		FSGRuntimeData& RuntimeData = GameInstance->GetMutableRuntimeData();
		RuntimeData.SpawnPointTag = SavePointTag;
		RuntimeData.bOpenShopOnLoad = bOpenMenuOnLoad;
	}

	RestartGame();
	
}

void AMyGameModeBase::PostBeginPlay()
{

	
	GetWorld()->OnWorldBeginPlay.RemoveAll(this);



	

}

void AMyGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// if (TestSavepoint)
	// {
	// 	TestSavepoint->OpenSaveUI(GetPlayerController());
	// }
}

void AMyGameModeBase::RegisterSpawner(ASGSpawnerActor* Spawner)
{
	SpawnerActors.Add(Spawner);
}

void AMyGameModeBase::RemoveSpawner(ASGSpawnerActor* SpawnerActor)
{
	if (SpawnerActors.Contains(SpawnerActor))
	{
		SpawnerActors.Remove(SpawnerActor);
	}
}

void AMyGameModeBase::SpawnActorFromSpawners()
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

	if (ASCharacterBase* NewlySpawnedCharacter =  Cast<ASCharacterBase>(NewlySpawnedActor))
	{
		UMyAttributeSet* AttributeSet = NewlySpawnedCharacter->GetAttributeSet();
		AttributeSet->MultiplyStats(CurrentDifficulty);
	}
}

void AMyGameModeBase::IncreaseDifficulty()
{
	CurrentDifficulty += DifficultyIncreaseAmount;
}

float AMyGameModeBase::GetCurrentDifficulty() const
{
	return CurrentDifficulty;
}

IInterface_PostProcessVolume* AMyGameModeBase::GetGlobalPostProcessVolume() const
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

void AMyGameModeBase::AddTimestopMaterial()
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

	FSUtils::bIsTimestopped = true;

	FWeightedBlendable NewBlendable;
	NewBlendable.Weight = 1.0f;
	NewBlendable.Object = TimestopMaterialObject;
	
	Settings->WeightedBlendables.Array.Add(NewBlendable);
}

void AMyGameModeBase::RemoveTimestopMaterial()
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
		FSUtils::bIsTimestopped = false;
	}
}

void AMyGameModeBase::TogglePlayerAutoBattle()
{
	ASCharacterBase* Character = GetCharacter();
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
			PlayerAIController = GetWorld()->SpawnActor<APlayerAIController>(PlayerAIControllerTemplate);
			SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass);
		}

		
		//Character->GetCharacterMovementComponent()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayerAIController->Possess(Character);
	}

	bIsUsingAutoBattle = !bIsUsingAutoBattle;
}

AMyPlayerController* AMyGameModeBase::GetPlayerController() const
{
	return PlayerController;
}

APlayerAIController* AMyGameModeBase::GetPlayerAIController() const
{
	return PlayerAIController;
}

ASCharacterBase* AMyGameModeBase::GetCharacter() const
{
	return Cast<ASCharacterBase>(PlayerController->GetCurrentPawnBasedOnIndex());
}

void AMyGameModeBase::HandlePlayerStart(FVector Location, AActor* Player)
{
	FSUtils::SetSpringArmLagOnActor(Player, false);
		
	Player->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
	{
		FSUtils::SetSpringArmLagOnActor(Player, true);
	});
		
	//GetWorld()->UpdateLevelStreaming();
	GetWorld()->WorldComposition->UpdateStreamingState(Location);
	GetWorld()->FlushLevelStreaming();
}

void AMyGameModeBase::OnAllLevelsLoaded()
{

	bool bFoundPlacement = false;
#if WITH_EDITOR
	// Allow for "Play from here"
	if (USGameSingleton* Singleton = Cast<USGameSingleton>(GEngine->GameSingleton))
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
			if (const USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
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
					FSUtils::SetSpringArmLagOnActor(PlayerPawn, false);
					PlayerPawn->SetActorLocationAndRotation(Location, Rotation);
					bFoundPlacement = true;
				}
			}
	
			if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				GameInstance->GetMutableRuntimeData().bHasLoadedFirstTime = true;
	
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
					if (GameInstance->GetRuntimeData().bOpenShopOnLoad)
					{
						AMyPlayerController* Controller = GetPlayerController();
						SavePointToUse->OpenSaveUI(Controller);
					}
				}
				GameInstance->GetMutableRuntimeData().bOpenShopOnLoad = false;
			}
		}
	}


	OnSGAllLevelsLoaded.Broadcast();

	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnUIFadeInCompleted.AddDynamic(this, &AMyGameModeBase::OnFadeOutCompleted_Initialize);
		Subsystem->DoFadeOut();
	}
}

void AMyGameModeBase::OnStreamingLevelLoaded(FName LevelLoaded)
{
	m_CurrentlyLoadedLevels++;
	if (m_CurrentlyLoadedLevels == m_NumStreamingLevels)
	{
		OnAllLevelsLoaded();
	}

}

void AMyGameModeBase::OnFadeOutCompleted_Initialize()
{
	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnUIFadeInCompleted.RemoveDynamic(this, &AMyGameModeBase::OnFadeOutCompleted_Initialize);
	}

	// Hack to stop camera spring from messing up on teleport
	if (PlayerController)
	{
		if (APawn* Pawn = PlayerController->GetPawn())
		{
			FSUtils::SetSpringArmLagOnActor(Pawn, true);
		}
	}
}

void AMyGameModeBase::OnFadeInCompleted_RestartLevel()
{
	RestartGame(false);
}
