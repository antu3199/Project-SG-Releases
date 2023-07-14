// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAbilityActor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SoulsGame/Subsystems/SGHitboxSubsystem.h"


#include "SGProjectileActor.generated.h"

class UNiagaraComponent;
class UAbilityProjectile;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class SOULSGAME_API ASGProjectileActor : public ASGAbilityActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGProjectileActor();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
	UProjectileMovementComponent * GetProjectileMovement() { return ProjectileMovement;  }

	void DisableHomingAfterSeconds(float Seconds);

	virtual void DoDestroyEffectInPlace();

	UPROPERTY(EditAnywhere)
	float LifeTime = 5.0f;

	virtual void SetEffectContext(FSGEffectInstigatorContext InEffectContext) override;
protected:
	
	UFUNCTION()
	virtual void OnActorDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void OnHitboxTriggered(class USGHitboxObject* HitboxObject, const FSGHitboxOverlapContext& OverlapContext);
	
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


