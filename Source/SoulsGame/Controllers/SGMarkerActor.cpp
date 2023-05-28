#include "SGMarkerActor.h"

#include "Components/DecalComponent.h"


ASGMarkerActor::ASGMarkerActor(const FObjectInitializer& ObjectInitializer)
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	DecalComponent->SetupAttachment(Root);

	DecalComponent->DecalSize = FVector(100.0f, 100.0f, 500.0f);
}

void ASGMarkerActor::RemoveMarker()
{
	this->Destroy();
}

void ASGMarkerActor::SetDecalSize(float X, float Y)
{
	// Flip decal size
	// DecalComponent->DecalSize = FVector(X, Y, DecalComponent->DecalSize.Z);
	DecalComponent->DecalSize = FVector( DecalComponent->DecalSize.X, X, Y);
	DecalComponent->SendRenderTransform_Concurrent();
}
