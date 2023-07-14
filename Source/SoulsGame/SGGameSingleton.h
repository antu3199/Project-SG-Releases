#pragma once
#include "CoreMinimal.h"

#include "SGGameSingleton.generated.h"

UCLASS()
class SOULSGAME_API USGGameSingleton : public UObject
{
public:
	GENERATED_BODY()

	virtual void PostInitProperties();
	
	class USGEditorInfoObject* GetEditorObject() const;

#if WITH_EDITOR
	void OnPIE(const UWorld* EditorWorld);

	FName EditorSteamingStartingLevelPackageNameToLoad = FName();
	bool bHasWorldPlacement = false;

	FString EditorWorldName;

#endif

protected:
	UPROPERTY()
	class USGEditorInfoObject* EditorInfoObject = nullptr;
};
