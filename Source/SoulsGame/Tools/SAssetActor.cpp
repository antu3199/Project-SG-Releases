#include "SAssetActor.h"


ASAssetActor::ASAssetActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AssetComponent = CreateDefaultSubobject<USAssetComponent>(TEXT("SAssetComponent"));
}

USAssetComponent* ASAssetActor::GetAssetComponent() const
{
	return AssetComponent;
}
