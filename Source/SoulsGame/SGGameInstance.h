
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"

#include "SGGameInstance.generated.h"

class USGMarkerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelProgressionSet, FGameplayTag, Tag);

UCLASS(config=Game, transient, BlueprintType, Blueprintable)
class SOULSGAME_API USGGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	//~ Begin UGameInstance interface
	virtual void Init() override;
	virtual void PostInitProperties() override;
	virtual void OnStart() override;

	virtual void Shutdown() override;

	// ~ End

	void OnMapBeginPlay(class ASGGameModeBase* GameModeBase);

	// TODO: Maybe this deserves its own subsystem
	UFUNCTION(BlueprintCallable)
	void SetLevelProgression(UPARAM(meta = (Categories = "Progression.Level")) FGameplayTag Tag);

	UPROPERTY(BlueprintAssignable)
	FOnLevelProgressionSet OnLevelProgressionSet;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USGMarkerController> MarkerControllerTemplate = nullptr;
	
	USGMarkerController* GetMarkerController() const;

#if WITH_EDITOR
	FVector GetEditorSpawnLocation() const;
#endif

	
	//void SetInitialPlayerLocation(AActor* PlayerActor);
	
	static bool Debug_ShowHitboxes;


	UFUNCTION(BlueprintCallable)
	class USGSaveLoadManager* GetSaveLoadManager();
	const class UStaticSGData* GetStaticData() const
	{
		return StaticData;
	}

	class UStaticSGData* GetStaticDataMutable() const
	{
		return StaticData;
	}



private:
	UFUNCTION()
	void OnFirstStreamingLevelLoaded();

	UFUNCTION()
	void OnLevelLoadedTest();

	UFUNCTION()
	void OnPostBeginPlayGame();

	UFUNCTION(Exec)
	void Debug_SaveGame();

	UFUNCTION(Exec)
	void Debug_LoadGame();

	UFUNCTION(Exec)
	void Debug_LoadEmptyGame();
	
	UFUNCTION(Exec)
	void Debug_ClearSaveData();

	UFUNCTION(Exec)
	void Debug_LoadAllStreamingLevels();

	UFUNCTION(Exec)
	void Debug_TestRestartLevelAtLocation();

	UFUNCTION()
	void Debug_StreamingFunction(int32 Linkage);

	void SetupRuntimeData();

#if WITH_EDITOR
	int32 FirstStreamingLevelIndex = -1;
	FVector EditorStartLocation;
#endif

	UPROPERTY(EditAnywhere)
	class UStaticSGData* StaticData;
	
	UPROPERTY(Transient)
	TObjectPtr<USGMarkerController> MarkerController = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<class USGSaveLoadManager> SaveLoadManager = nullptr;

};


