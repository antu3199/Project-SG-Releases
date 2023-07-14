#pragma once
#include "CoreMinimal.h"

#include "SGMovementOverrideComponent.generated.h"

class USGCharacterMovementComponent;

UCLASS()
class USGMovementOverrideComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USGMovementOverrideComponent();

	virtual void PostInitProperties() override;

	
protected:
	UPROPERTY(Transient)
	USGCharacterMovementComponent* MovementComponent = nullptr;
};
