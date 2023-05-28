#pragma once
#include "CoreMinimal.h"

#include "SGPowerupActor.generated.h"
UCLASS(Abstract, Blueprintable)
class ASGPowerupActor : public AActor
{
	GENERATED_BODY()

	ASGPowerupActor();
public:
	class USGPowerupComponent* GetPowerupComponent() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USGPowerupComponent* PowerupComponent = nullptr;
};
