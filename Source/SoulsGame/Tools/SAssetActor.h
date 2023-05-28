#pragma once
#include "CoreMinimal.h"
#include "SAssetComponent.h"
#include "GameFramework/Actor.h"

#include "SAssetActor.generated.h"

UCLASS()
class SOULSGAME_API ASAssetActor : public AActor
{
public:
	GENERATED_BODY()

public:
	ASAssetActor(const FObjectInitializer& ObjectInitializer);
	
	// Returns the actor's houdini component.
	USAssetComponent* GetAssetComponent() const;

private:
	// Pointer to the root HoudiniAssetComponent
	UPROPERTY(Category = SAssetActor, VisibleAnywhere, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|HoudiniEngine")/*, AllowPrivateAccess = "true"*/)
	USAssetComponent * AssetComponent;
};
