// Fill out your copyright notice in the Description page of Project Settings.
// THIS CLASS IS DEPRECATED - USE THE POWERUP VERSION INSTEAD!
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulsGame/Abilities/MyGameplayAbility.h"
#include "SoulsGame/Abilities/PowerUps/Components/Spawned/ProjectileActor.h"

#include "ProjectileShooterComponent.generated.h"
DECLARE_MULTICAST_DELEGATE(FOnShotFinished);
DECLARE_MULTICAST_DELEGATE(FOnShootingFinished);
DECLARE_MULTICAST_DELEGATE(FOnPreShot);

UCLASS( Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSGAME_API UProjectileShooterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileShooterComponent();

	FGameplayAbilitySpecWrapper* AbilityWrapper;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UMyGameplayEffect>> DebugGameplayEffects;

	void ActivateDebugEffects(UMyGameplayAbility* InAbility);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	virtual void DoShot();

	// If false, don't shoot
	virtual bool PreShot();

	virtual void OnEndBarrage();


	// Does a single shot
	void SingleShot(const FVector& OverrideDirection = FVector::ZeroVector);

	void SetNextShotAsLast();

	USceneComponent * ProjectileOrigin;

	void DoBarrage(float TimeBetweenShots, float NumShots);
	
	FTimerHandle ShootingHandle;
	FTimerHandle LoopHandle;
	FTimerHandle BarrageHandle;

	void Helper_SetProjectileToDirection(class UProjectileMovementComponent * MovementComponent, FVector& LocalDirection);
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject);

	FVector GetRelativeCharacterForward() const;
	FVector GetRelativeCharacterRight() const;
	FVector GetRelativeCharacterUp() const;


	FVector GetCameraRelativeVector(const FVector& Direction) const;
	FVector GetCameraRelativeRight() const;
	FVector GetCameraRelativeUp() const;
	
	UPROPERTY(EditAnywhere)
	bool IsHomingProjectile;

	UPROPERTY(EditAnywhere)
	float HomingAccelerationMagnitude = 0;

	UPROPERTY()
	USceneComponent * Target = nullptr;

	UPROPERTY()
	USceneComponent * HomingTarget = nullptr;

	FVector TargetVector = FVector::ZeroVector;
	bool bUseTargetVector;
	

	UPROPERTY(EditAnywhere)
	float DisableHomingAfterSeconds = 1;

	UPROPERTY(EditAnywhere)
	FVector LocalShootDirection = FVector(0, 0, 1);

	FVector TransformOffset = FVector::ZeroVector;

	TWeakPtr<FGameplayEffectsWrapperContainer> GameplayEffectsContainer = nullptr;

public:

	void Initialize(USceneComponent * Origin);

	void SetTarget(USceneComponent * InTarget);
	void SetTargetVector(FVector InTarget);

	void SetHomingTarget(USceneComponent * InTarget);

	virtual void SetLevel(int32 Level);
	int32 GetLevel() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectileActor> ProjectileActorTemplate;

	// TODO: Add a bunch of parameters here
	virtual void StartShooting();

	virtual void StopShooting();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnTargetUpdated(const FVector& Direction);
	
	FVector GetShooterToTargetDirection() const;
	FVector GetShooterToTargetDirection(const FVector& InTarget) const;
	FVector GetShooterToTargetDirection(USceneComponent * InTarget) const;

	UPROPERTY(EditAnywhere)
	bool bPlayLooping = true;

	UPROPERTY(EditAnywhere)
	float LoopDelay = 1;

	UPROPERTY(EditAnywhere)
	float LoopForSeconds = 3;
	
	
	bool IsShooting = false;

	void SetGameplayEffectsUsingContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container);
	void ClearEffects();

	FOnShotFinished OnShotFinished;
	FOnShootingFinished OnShootingFinished;
	FOnPreShot OnPreShot;
	bool bNextShotAsLast = false;

	FVector ShootDirection;

	// If networked game, need to refactor this
	int32 CurrentLevel = 0;
	
};
