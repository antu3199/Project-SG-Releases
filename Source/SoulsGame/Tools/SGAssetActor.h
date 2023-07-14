#pragma once
#include "CoreMinimal.h"
#include "SGAssetComponent.h"
#include "GameFramework/Actor.h"

#include "SGAssetActor.generated.h"

UCLASS()
class SOULSGAME_API ASGAssetActor : public AActor
{
public:
	GENERATED_BODY()

public:
	ASGAssetActor(const FObjectInitializer& ObjectInitializer);
	
	// Returns the actor's houdini component.
	USGAssetComponent* GetAssetComponent() const;

private:
	// Pointer to the root HoudiniAssetComponent
	UPROPERTY(Category = SAssetActor, VisibleAnywhere, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|HoudiniEngine")/*, AllowPrivateAccess = "true"*/)
	USGAssetComponent * AssetComponent;
};
