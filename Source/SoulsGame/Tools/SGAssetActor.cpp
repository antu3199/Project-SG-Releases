#include "SGAssetActor.h"


ASGAssetActor::ASGAssetActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AssetComponent = CreateDefaultSubobject<USGAssetComponent>(TEXT("SAssetComponent"));
}

USGAssetComponent* ASGAssetActor::GetAssetComponent() const
{
	return AssetComponent;
}
