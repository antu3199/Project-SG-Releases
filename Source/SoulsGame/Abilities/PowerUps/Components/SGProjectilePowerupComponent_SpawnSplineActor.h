#pragma once
#include "CoreMinimal.h"
#include "SGPowerupComponent.h"
#include "Spawned/SGSplineActor.h"
#include "SGProjectilePowerupComponent_SpawnSplineActor.generated.h"


UCLASS(Blueprintable)
class USGSpawnSplineActorPowerupComponent : public USGPowerupComponent
{
	GENERATED_BODY()

public:
	USGSpawnSplineActorPowerupComponent();
	
	virtual void OnInitializeAbilityContext(class USGPowerupAbility* Ability, AActor* InAbilitySystemOwner, AActor* InComponentOwner) override;
	
protected:
	virtual void DoPowerup() override;
	
	virtual void StopPowerup();

	void OnComponentObjectFinished();

	float GetLoopDelay();
	
	UPROPERTY(EditAnywhere)
	FVector TransformOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASGSplineActor> SpawnedSplineActorTemplate;

	UPROPERTY(EditAnywhere)
	bool bAttachToParent = true;

	
	UPROPERTY(Transient)
	ASGSplineActor* CurrentSpawnedActor = nullptr;

	FTimerHandle ShootingHandle;

	double LastShotTime = 0;
	bool bHasStarted = false;

	float CachedDuration = 0;
};
