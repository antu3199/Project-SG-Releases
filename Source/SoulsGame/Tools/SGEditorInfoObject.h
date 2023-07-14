#pragma once

#include "CoreMinimal.h"

#include "SGEditorInfoObject.generated.h"

UCLASS()
class SOULSGAME_API USGEditorInfoObject : public UObject
{
public:
	GENERATED_BODY()

	UPROPERTY()
	UObject* InputObject = nullptr;

	UPROPERTY()
	FString JsonObjectPath;

	void SetInputObject(UObject * Other);
	void SetJsonObjectPath(FString Path);
};
