#pragma once
#include "CoreMinimal.h"
#include "SimpleLevelManagerSubsystem.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnLoadStreamingLevelFinished, FName, LevelName );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnUnloadStreamingLevelFinished, FName, LevelName );
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllLevelsLoaded);


// USimpleLevelManagerSubsystem
UCLASS()
class USimpleLevelManagerSubsystem : public UGameInstanceSubsystem
{
   GENERATED_BODY()

public:

   //~UGameInstanceSubsystem interfaces
   virtual void Initialize( FSubsystemCollectionBase &Collection ) override;
   virtual void Deinitialize() override;
   //~End of UGameInstanceSubsystem interfaces

   // Load or unload levels
   bool LoadStreamingLevel( FName LevelName, bool bMakeVisible );
   void UnloadStreamingLevel( FName LevelName );

   void LoadAllStreamingLevels();

   UPROPERTY(BlueprintAssignable)
   FOnLoadStreamingLevelFinished OnLoadStreamingLevelFinished;

   UPROPERTY(BlueprintAssignable)
   FOnUnloadStreamingLevelFinished OnUnloadStreamingLevelFinished;

   UPROPERTY(BlueprintAssignable)
   FOnAllLevelsLoaded OnAllLevelsLoaded;
private:

   void OnWorldDestroyed( UWorld *World );

   UFUNCTION()
   void OnLoadStreamLevelFinished( int32 Linkage );

   UFUNCTION()
   void OnUnloadStreamLevelFinished( int32 Linkage );

   UFUNCTION()
   void OnStreamingLevelLoadedForLoadAll(FName Name);

   // Arrays of pending levels
   struct FLevelRefCounter
   {
      FName                                LevelName;
      int32                                Linkage;
      int32                                RefCount = 0;
      bool                                 bIsActive;
      bool                                 bWaitingForLevel;
      bool                                 bVisible;
   };

   // Active Levels and the NextUUID
   TArray< FLevelRefCounter > ActiveLevels;
   int32 NextUUID = 1;

   int32 PendingLoadingLevels = 0;
   int32 CurrentlyLoadedPendingLevels = 0;

   TFunction<void()> OnSteamingLevelsLoadedCallback;
   TArray<FOnLoadStreamingLevelFinished> LoadAllStreamingLevelFinishedDelegates;
};