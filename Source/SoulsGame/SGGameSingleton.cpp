#include "SGGameSingleton.h"

#include "Engine/LevelStreaming.h"
#include "Tools/SGEditorInfoObject.h"
#include "UObject/UObjectGlobals.h"


void USGGameSingleton::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UPackage * Package = GetPackage();
		if (EditorInfoObject == nullptr)
		{
			EditorInfoObject = NewObject<USGEditorInfoObject>(Package);
		}
	}
}

USGEditorInfoObject* USGGameSingleton::GetEditorObject() const
{
	return EditorInfoObject;
}

#if WITH_EDITOR
void USGGameSingleton::OnPIE(const UWorld* EditorWorld)
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
}

#endif
