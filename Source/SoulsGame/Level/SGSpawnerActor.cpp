#include "SGSpawnerActor.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameModeBase.h"

AActor* ASGSpawnerActor::SpawnRandomActorFromTemplate()
{
	if (SpawnedActorTemplates.Num() == 0)
	{
		return nullptr;
	}
	
	const int32 Index = FMath::RandRange(0, SpawnedActorTemplates.Num() - 1);
	return SpawnActor(SpawnedActorTemplates[Index]);
}

AActor* ASGSpawnerActor::SpawnActor(const TSubclassOf<AActor>& Actor)
{
	// TODO: Have the ability to change rotation of spawned actor
	const FTransform Transform(FRotator::ZeroRotator,GetActorLocation(), FVector::OneVector);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = SpawningCollisionMethod; //Default
	SpawnParameters.Owner = this;
	AActor * SpawnedObject = GetWorld()->SpawnActor<AActor>(Actor, Transform, SpawnParameters);
	if (SpawnedObject)
	{
		SpawnedActors.Add(SpawnedObject);
	}

	return SpawnedObject;
}

void ASGSpawnerActor::BeginPlay()
{
	Super::BeginPlay();

	if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->RegisterSpawner(this);
	}
}

void ASGSpawnerActor::BeginDestroy()
{
	if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->RemoveSpawner(this);
	}
	
	Super::BeginDestroy();
}
