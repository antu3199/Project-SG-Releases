#include "SGAssetComponent.h"

USGAssetComponent::USGAssetComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGAssetComponent::SetInputObject(UObject* Other)
{
	InputObject = Other;
}

void USGAssetComponent::SetJsonObjectPath(FString Path)
{
	JsonObjectPath = Path;
}
