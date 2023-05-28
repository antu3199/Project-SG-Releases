#include "SGameSingleton.h"

#include "SHitboxManager.h"
#include "Engine/LevelStreaming.h"
#include "Tools/SEditorInfoObject.h"
#include "UObject/UObjectGlobals.h"


void USGameSingleton::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UPackage * Package = GetPackage();
		if (EditorInfoObject == nullptr)
		{
			EditorInfoObject = NewObject<USEditorInfoObject>(Package);
		}
		
		HitboxManager = NewObject<USHitboxManager>(this);
	}
	

}

USEditorInfoObject* USGameSingleton::GetEditorObject() const
{
	return EditorInfoObject;
}

USHitboxManager* USGameSingleton::GetHitboxManager() const
{
	return HitboxManager;
}

#if WITH_EDITOR
void USGameSingleton::OnPIE(const UWorld* EditorWorld)
{
	EditorWorldName = GetNameSafe(EditorWorld);
	
	const TArray<ULevelStreaming*>& Levels = EditorWorld->GetStreamingLevels();
	if (Levels.Num() > 0)
	{
		for (int32 i = 0; i < Levels.Num(); i++)
		{
			const bool bHasLoadedLevel = Levels[i]->HasLoadedLevel();
			if (bHasLoadedLevel)
			{
				EditorSteamingStartingLevelPackageNameToLoad = Levels[i]->PackageNameToLoad;
				break;
			}
		}
	}
	
	int32 Test = 0;
}

#endif
