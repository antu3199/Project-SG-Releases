#include "SGSimpleLevelManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreaming.h"

// ULevelManagerSubsystem
// https://forums.unrealengine.com/t/level-streaming-with-c/8823/11


// Initialize
void
USGSimpleLevelManagerSubsystem::Initialize( FSubsystemCollectionBase &Collection )
{
   Super::Initialize( Collection );

   GEngine->OnWorldDestroyed().AddUObject( this, &ThisClass::OnWorldDestroyed );
}

// World Destroyed
void
USGSimpleLevelManagerSubsystem::OnWorldDestroyed( UWorld *World )
{
   ActiveLevels.Empty();
}

// Initialize -- remove the ticker
void
USGSimpleLevelManagerSubsystem::Deinitialize()
{
   // Disallow level streaming while shutting down
   UGameplayStatics::FlushLevelStreaming( GetWorld() );

   GEngine->OnWorldDestroyed().RemoveAll( this );

   // All active levels should be unloaded here...
   for ( int32 idx = 0; idx < ActiveLevels.Num(); ++idx )
   {
      // Active levels need to be unloaded
      if ( ActiveLevels[ idx ].bIsActive )
      {
         // Ensure all levels are removed
         ActiveLevels[ idx ].RefCount = 1;
         UnloadStreamingLevel( ActiveLevels[ idx ].LevelName );
      }
   }

   Super::Deinitialize();
}

// Load a streaming level
bool
USGSimpleLevelManagerSubsystem::LoadStreamingLevel( FName LevelName, bool bMakeVisible)
{
   // Possible that this is the World that we are on now... if so, we aren't streaming...
   if ( *UGameplayStatics::GetCurrentLevelName( this ) == LevelName )
   {
      OnLoadStreamingLevelFinished.Broadcast(LevelName);
      return true;
   }

   // Handle incrementing the ref count
   for ( int32 idx = 0; idx < ActiveLevels.Num(); ++idx )
   {
      if ( ActiveLevels[ idx ].LevelName == LevelName )
      {
         ActiveLevels[ idx ].RefCount++;
         OnLoadStreamingLevelFinished.Broadcast(LevelName);
         return true;
      }
   }

   // Make sure this level exists
   ULevelStreaming *LevelStreaming = UGameplayStatics::GetStreamingLevel( GetWorld(), LevelName );
   if ( !LevelStreaming )
   {
      return false;
   }

   FLatentActionInfo LatentInfo;
   LatentInfo.CallbackTarget = this;
   LatentInfo.UUID = NextUUID++;
   LatentInfo.Linkage = LatentInfo.UUID;
   LatentInfo.ExecutionFunction = "OnLoadStreamLevelFinished";

   int32 idx = ActiveLevels.Add( FLevelRefCounter { LevelName, LatentInfo.UUID, 1, false, true, bMakeVisible } );

   // Load and activate this level
   UGameplayStatics::LoadStreamLevel( GetWorld(), LevelName, bMakeVisible, false, LatentInfo );

   return true;
}



// Unload the Streaming Level
void
USGSimpleLevelManagerSubsystem::UnloadStreamingLevel( FName LevelName)
{
   ULevelStreaming *LevelStreaming = UGameplayStatics::GetStreamingLevel( GetWorld(), LevelName );
   if ( !LevelStreaming )
   {
      OnUnloadStreamingLevelFinished.Broadcast(LevelName);
      return;
   }

   // Check active levels and remove this level
   for ( int32 idx = 0; idx < ActiveLevels.Num(); ++idx )
   {
      if ( ActiveLevels[ idx ].LevelName == LevelName )
      {
         ActiveLevels[ idx ].RefCount--;
         if ( ActiveLevels[ idx ].RefCount <= 0 )
         {
            FLatentActionInfo LatentInfo;
            LatentInfo.CallbackTarget = this;
            LatentInfo.UUID = ActiveLevels[ idx ].Linkage;
            LatentInfo.Linkage = LatentInfo.UUID;
            LatentInfo.ExecutionFunction = "OnUnloadStreamLevelFinished";

            UGameplayStatics::UnloadStreamLevel( GetWorld(), LevelName, LatentInfo, false );
         }
         else
         {
            OnUnloadStreamingLevelFinished.Broadcast(LevelName);
         }
         break;
      }
   }
}

void USGSimpleLevelManagerSubsystem::LoadAllStreamingLevels()
{

   /*
   const TArray<ULevelStreaming*>& Levels = GetWorld()->GetStreamingLevels();
   PendingLoadingLevels = Levels.Num();
   CurrentlyLoadedPendingLevels = 0;
   LoadAllStreamingLevelFinishedDelegates.Reset();
   OnSteamingLevelsLoadedCallback = OnSteamingLevelsLoaded;

   for (int32 i = 0; i < Levels.Num(); i++)
   {
      const ULevelStreaming* LevelToLoad = Levels[i];
      const FName LevelName =  LevelToLoad->GetWorldAssetPackageFName();

      FOnLoadStreamingLevelFinished& Delegate = LoadAllStreamingLevelFinishedDelegates.Add_GetRef(FOnLoadStreamingLevelFinished());
      Delegate.BindUObject(this, &USimpleLevelManagerSubsystem::OnStreamingLevelLoadedForLoadAll);

      LoadStreamingLevel(LevelName, false, MoveTemp(Delegate));
   }
   */
   const TArray<ULevelStreaming*>& Levels = GetWorld()->GetStreamingLevels();
   PendingLoadingLevels = Levels.Num();
   CurrentlyLoadedPendingLevels = 0;
   LoadAllStreamingLevelFinishedDelegates.Reset();

   for (int32 i = 0; i < Levels.Num(); i++)
   {
      const ULevelStreaming* LevelToLoad = Levels[i];
      const FName LevelName =  LevelToLoad->GetWorldAssetPackageFName();
      LoadStreamingLevel(LevelName, true);
   }
   
}

// Load Streaming Level Callback
void
USGSimpleLevelManagerSubsystem::OnLoadStreamLevelFinished( int32 Linkage )
{
   for ( int32 idx = 0; idx < ActiveLevels.Num(); ++idx )
   {
      // Matching?
      if ( ActiveLevels[ idx ].Linkage == Linkage )
      {
         ActiveLevels[ idx ].bIsActive = true;
         ActiveLevels[ idx ].bWaitingForLevel = false;
         OnLoadStreamingLevelFinished.Broadcast(ActiveLevels[ idx ].LevelName);
         CurrentlyLoadedPendingLevels++;
         if (CurrentlyLoadedPendingLevels == PendingLoadingLevels)
         {
            OnAllLevelsLoaded.Broadcast();
         }
      }
   }
}


// Unload Streaming Level Callback
void
USGSimpleLevelManagerSubsystem::OnUnloadStreamLevelFinished( int32 Linkage )
{
   for ( int32 idx = 0; idx < ActiveLevels.Num(); ++idx )
   {
      // Matching?
      if ( ActiveLevels[ idx ].Linkage == Linkage )
      {
         OnUnloadStreamingLevelFinished.Broadcast(ActiveLevels[ idx ].LevelName);
         ActiveLevels.RemoveAt( idx );
         return;
      }
   }
}

void USGSimpleLevelManagerSubsystem::OnStreamingLevelLoadedForLoadAll(FName Name)
{
   CurrentlyLoadedPendingLevels++;

   if (CurrentlyLoadedPendingLevels == PendingLoadingLevels)
   {
      OnSteamingLevelsLoadedCallback();
      LoadAllStreamingLevelFinishedDelegates.Reset();
   }
}
