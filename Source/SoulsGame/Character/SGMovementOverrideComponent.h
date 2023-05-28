#pragma once
#include "CoreMinimal.h"

#include "SGMovementOverrideComponent.generated.h"

class UMyCharacterMovementComponent;

UCLASS()
class USGMovementOverrideComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USGMovementOverrideComponent();

	virtual void PostInitProperties() override;

	
protected:
	UPROPERTY(Transient)
	UMyCharacterMovementComponent* MovementComponent = nullptr;
};
