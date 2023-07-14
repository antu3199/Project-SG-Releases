#pragma once
#include "CoreMinimal.h"
#include "SGLootActor.h"
#include "GameFramework/Actor.h"
#include "SGLootActorEXP.generated.h"

UCLASS(Abstract)
class ASGLootActorEXP : public ASGLootActor
{
	GENERATED_BODY()

public:
	virtual void ApplyLootToCharacter(class ASGCharacterBase* Character) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
