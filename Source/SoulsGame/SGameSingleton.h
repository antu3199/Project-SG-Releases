#pragma once
#include "CoreMinimal.h"

#include "SGameSingleton.generated.h"
class USHitboxManager;

UCLASS()
class SOULSGAME_API USGameSingleton : public UObject
{
public:
	GENERATED_BODY()

	virtual void PostInitProperties();
	
	class USEditorInfoObject* GetEditorObject() const;

	USHitboxManager* GetHitboxManager() const;

#if WITH_EDITOR
	void OnPIE(const UWorld* EditorWorld);

	FName EditorSteamingStartingLevelPackageNameToLoad = FName();
	bool bHasWorldPlacement = false;

	FString EditorWorldName;

#endif

	
	
protected:
	UPROPERTY(Transient)
	USHitboxManager* HitboxManager = nullptr;
	
	UPROPERTY()
	class USEditorInfoObject* EditorInfoObject = nullptr;
};
