#include "FloatingTextActor.h"

AFloatingTextActor::AFloatingTextActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFloatingTextActor::BeginDestroy()
{
	Super::BeginDestroy();
}

void AFloatingTextActor::BeginPlay()
{
	Super::BeginPlay();

	AnchorLocation = GetActorLocation();
}

void AFloatingTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
