#pragma once
#include "CoreMinimal.h"

class SOULSGAME_API SGBlueprintLibrary : public UBlueprintFunctionLibrary
{
public:

	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static float GetDistanceFromCamera(const UObject* WorldContextObject, int32 PlayerIndex, const FVector& Location);
};
