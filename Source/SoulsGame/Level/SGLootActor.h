#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGLootActor.generated.h"

UCLASS(Abstract)
class ASGLootActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void ApplyLootToCharacter(class ASCharacterBase* Character);
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
