#pragma once
#include "CoreMinimal.h"
#include "SGPowerupComponent.h"
#include "SGPowerupStats.h"
#include "SGProjectilePowerupComponent.generated.h"


UCLASS(Blueprintable)
class USGProjectilePowerupComponent : public USGPowerupComponent
{
	GENERATED_BODY()

public:
	USGProjectilePowerupComponent();
	virtual void OnInitializeAbilityContext(class USGPowerupAbility* Ability, AActor* InAbilitySystemOwner, AActor* InComponentOwner) override;


	//void Initialize(USceneComponent * Origin);

	void SetHomingTarget(USceneComponent * InTarget);

	UFUNCTION(BlueprintCallable)
	void ResetPowerupTimer();

	USceneComponent * GetProjectileOrigin() const;
	
protected:

	virtual void DoPowerup() override;

	FVector GetShooterToTargetDirection() const;
	FVector GetShooterToTargetDirection(const FVector& InTarget) const;
	FVector GetShooterToTargetDirection(USceneComponent * InTarget) const;
	
	// Does a single shot
	void SingleShot(const FVector& OverrideDirection = FVector::ZeroVector);

	void DoBarrage(float TimeBetweenShots, float NumShots);
	virtual void OnEndBarrage();

	FTimerHandle BarrageHandle;

	virtual void OnShotSpawn(class AProjectileActor* SpawnedObject);

	UPROPERTY(EditAnywhere)
	bool IsHomingProjectile;

	UPROPERTY(EditAnywhere)
	float HomingAccelerationMagnitude = 0;

	UPROPERTY()
	USceneComponent * HomingTarget = nullptr;

	UPROPERTY(EditAnywhere)
	float DisableHomingAfterSeconds = 1;

	UPROPERTY(EditAnywhere)
	FVector LocalShootDirection = FVector(0, 0, 1);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectileActor> ProjectileActorTemplate;
	
	UPROPERTY(Transient)
	USceneComponent * ProjectileOrigin = nullptr;
	

	FVector TransformOffset = FVector::ZeroVector;
	
	FVector ShootDirection;


};
