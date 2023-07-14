#pragma once
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"

#include "SGAssetComponent.generated.h"

UCLASS()
class SOULSGAME_API USGAssetComponent : public UPrimitiveComponent
{
public:
	GENERATED_BODY()

	USGAssetComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	UObject* InputObject = nullptr;

	UPROPERTY()
	FString JsonObjectPath;

	void SetInputObject(UObject * Other);
	void SetJsonObjectPath(FString Path);
};
