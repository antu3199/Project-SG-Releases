#pragma once
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"

#include "SAssetComponent.generated.h"

UCLASS()
class SOULSGAME_API USAssetComponent : public UPrimitiveComponent
{
public:
	GENERATED_BODY()

	USAssetComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	UObject* InputObject = nullptr;

	UPROPERTY()
	FString JsonObjectPath;

	void SetInputObject(UObject * Other);
	void SetJsonObjectPath(FString Path);
};
