#pragma once
#include "CoreMinimal.h"

#include "SGMarkerActor.generated.h"

UCLASS()
class ASGMarkerActor : public AActor
{
	GENERATED_BODY()
public:

	ASGMarkerActor(const FObjectInitializer& ObjectInitializer);
	void RemoveMarker();

	void SetDecalSize(float X, float Y);
	
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDecalComponent* DecalComponent;
};
