#include "SAssetComponent.h"

USAssetComponent::USAssetComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USAssetComponent::SetInputObject(UObject* Other)
{
	InputObject = Other;
}

void USAssetComponent::SetJsonObjectPath(FString Path)
{
	JsonObjectPath = Path;
}
