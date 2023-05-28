// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GameFramework/GameModeBase.h"
#include "Level/SGSpawnerActor.h"
#include "MyGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSGAllLevelsLoaded);


/**
 * 
 */
UCLASS()
class SOULSGAME_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;
	virtual  void BeginDestroy() override;

	virtual void PostInitProperties() override;
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void RestartGame(bool bFadeIn = true);


	UFUNCTION(BlueprintCallable)
	void RestartGameAtSavePoint(const FGameplayTag& SavePointTag, bool bOpenMenuOnLoad = false);

	void PostBeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	TArray<ASGSpawnerActor*> GetSpawnerActors() const
	{
		return SpawnerActors;
	}

	void RegisterSpawner(ASGSpawnerActor* Spawner);
	void RemoveSpawner(ASGSpawnerActor* SpawnerActor);

	void SpawnActorFromSpawners();
	void IncreaseDifficulty();

	UFUNCTION(BlueprintCallable)
	float GetCurrentDifficulty() const;

	UPROPERTY(EditAnywhere)
	bool bDisableSpawning = false;

	UPROPERTY(EditAnywhere)
	FGameplayTag Debug_SpawnPoint;
	
	IInterface_PostProcessVolume* GetGlobalPostProcessVolume() const;

	void AddTimestopMaterial();
	void RemoveTimestopMaterial();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FWeightedBlendable, meta=( AllowedClasses="BlendableInterface", Keywords="PostProcess" ))
	TObjectPtr<UObject> TimestopMaterialObject;


	UFUNCTION(BlueprintCallable)
	void TogglePlayerAutoBattle();

	UFUNCTION(BlueprintCallable)
	class AMyPlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable)
	class APlayerAIController* GetPlayerAIController() const;

	UFUNCTION(BlueprintCallable)
	class ASCharacterBase* GetCharacter() const;

	UPROPERTY(BlueprintAssignable)
	FOnSGAllLevelsLoaded OnSGAllLevelsLoaded;

	bool HasHandledPlayerStart() const { return bHandledPlayerStart; }

	void HandlePlayerStart(FVector Location, AActor* Player);
	
private:
	void OnAllLevelsLoaded();

	UFUNCTION()
	void OnStreamingLevelLoaded(FName LevelLoaded);

	UFUNCTION()
	void OnFadeOutCompleted_Initialize();

	UFUNCTION()
	void OnFadeInCompleted_RestartLevel();
	
	bool bWasFirstMapGame() const;
	
	UPROPERTY(EditAnywhere)
	float SpawnRate = 5.0f;

	UPROPERTY(EditAnywhere)
	float DifficultyIncreaseSeconds = 5.0f;

	UPROPERTY(EditAnywhere)
	float DifficultyIncreaseAmount = 0.2f;

	bool bIsUsingAutoBattle = false;
	
	UPROPERTY(EditAnywhere, Category = "Classes")
	TSubclassOf<class APlayerAIController> PlayerAIControllerTemplate;
	
	UPROPERTY(Transient)
	APlayerAIController * PlayerAIController = nullptr;

	UPROPERTY(Transient)
	AMyPlayerController* PlayerController = nullptr;

	UPROPERTY(Transient)
	ASpectatorPawn* SpectatorPawn = nullptr;
	
	UPROPERTY()
	TArray<ASGSpawnerActor*> SpawnerActors;

	float CurrentDifficulty = 1.0f;
	
	FTimerHandle SpawnTimerHandle;
	FTimerHandle DifficultyIncreaseTimerHandle;


	bool bHandledPlayerStart = false;

	FGameplayTag m_SavepointTag;

	int32 m_NumStreamingLevels = 0;
	int32 m_CurrentlyLoadedLevels = 0;
	
};
