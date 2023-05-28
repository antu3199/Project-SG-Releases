#include "SGPowerupActor.h"

#include "SGPowerupComponent.h"

ASGPowerupActor::ASGPowerupActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

USGPowerupComponent* ASGPowerupActor::GetPowerupComponent() const
{
	return PowerupComponent;
}

void ASGPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	// Define powerup component in Blueprints for flexibility
	PowerupComponent = Cast<USGPowerupComponent>(GetComponentByClass(USGPowerupComponent::StaticClass()));
}
