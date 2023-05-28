#pragma once

#define INVALID_MARKER_ID  -1

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class ASGMarkerActor;

#include "SGMarkerController.generated.h"


USTRUCT()
struct SOULSGAME_API FMarkerInfo
{
	GENERATED_BODY()

	FMarkerInfo();
	FMarkerInfo(int32 InMarkerId);
	
	int32 MarkerId = INVALID_MARKER_ID;

	UPROPERTY(Transient)
	ASGMarkerActor* MarkerActor = nullptr;
	TWeakObjectPtr<UWorld> World = nullptr;
};

UCLASS(Blueprintable)
class SOULSGAME_API USGMarkerController : public UObject
{
	GENERATED_BODY()
public:
	static USGMarkerController* GetSingleton(const UObject* WorldContextObject);
	void Initialize();

	int32 CreateNewMarker(FVector MarkerLocation, FVector2f Size = FVector2f::One(), bool bCalculateGroundPosition = true);

	void RemoveMarker(int32 MarkerId);

private:
	int32 RunningMarkerId = 0;
	TSet<int32> ReusableMarkerIds;

	UPROPERTY(Transient)
	TMap<int32, FMarkerInfo> MarkerObjects;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ASGMarkerActor> MarkerActorTemplate;
	
	//TArray<FUpgradeOption> UpgradePool;

};
