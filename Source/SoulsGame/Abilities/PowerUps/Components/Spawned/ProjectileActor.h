// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SoulsGame/Weapon/AbilityActor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"


#include "ProjectileActor.generated.h"

class UNiagaraComponent;
class UAbilityProjectile;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class SOULSGAME_API AProjectileActor : public AAbilityActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileActor();

	virtual void BeginPlay() override;
	

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
	
	UProjectileMovementComponent * GetProjectileMovement() { return ProjectileMovement;  }

	void DisableHomingAfterSeconds(float Seconds);

	virtual TWeakPtr<FGameplayEffectsWrapperContainer> GetGameplayEffectsWrapperContainer() override;

	virtual void DoDestroyEffectInPlace();

	UPROPERTY(EditAnywhere)
	float LifeTime = 5.0f;


protected:
	
	UFUNCTION()
	virtual void OnActorDestroyed(AActor* DestroyedActor);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent * ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent * NiagaraComponent;
	
	UPROPERTY(EditAnywhere)
	bool PlayNiagaraParticleSystem = true;

	UPROPERTY(EditAnywhere)
	bool PlayCascadeSystem = false;
	
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraCollisionTemplate;

	UPROPERTY(EditAnywhere)
	UParticleSystem* CascadeCollisionTemplate;

	UPROPERTY(EditAnywhere)
	bool bDestroyOnHit = true;
	
	
	FTimerHandle DisableHomingHandle;

	FTimerHandle DestroySelfHandle;



	void PlayParticleHitAtLocation(const FVector Location);
};
