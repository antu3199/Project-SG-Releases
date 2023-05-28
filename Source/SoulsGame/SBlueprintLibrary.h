#pragma once
#include "CoreMinimal.h"

class USHitboxManager;

class SOULSGAME_API SBlueprintLibrary : public UBlueprintFunctionLibrary
{
public:
	static USHitboxManager* GetHitboxManager();

	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static float GetDistanceFromCamera(const UObject* WorldContextObject, int32 PlayerIndex, const FVector& Location);
};
