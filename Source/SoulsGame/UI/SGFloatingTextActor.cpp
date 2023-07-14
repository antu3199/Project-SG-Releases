#include "SGFloatingTextActor.h"

ASGFloatingTextActor::ASGFloatingTextActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASGFloatingTextActor::BeginDestroy()
{
	Super::BeginDestroy();
}

void ASGFloatingTextActor::BeginPlay()
{
	Super::BeginPlay();

	AnchorLocation = GetActorLocation();
}

void ASGFloatingTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
