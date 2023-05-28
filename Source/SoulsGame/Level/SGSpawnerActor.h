#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGSpawnerActor.generated.h"

UCLASS(Abstract)
class ASGSpawnerActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> SpawnedActorTemplates;

	AActor* SpawnRandomActorFromTemplate();
	
	AActor* SpawnActor(const TSubclassOf<AActor>& Actor);

	UPROPERTY(EditAnywhere)
	ESpawnActorCollisionHandlingMethod SpawningCollisionMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UPROPERTY(Transient)
	TArray<AActor*> SpawnedActors;
};
