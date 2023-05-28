// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SoulsGame/Projectiles/ProjectileShooterComponent.h"

#include "GameplayAbilityBlueprint.h"
#include "NiagaraComponent.h"
#include "SoulsGame/Projectiles/ISShooter.h"
#include "SoulsGame/Weapon/AbilityActor.h"

#include "ConfectioniteCharacter.generated.h"

class UCharacterAbilitySystemComponent;
class UMyAttributeSet;
class ASHumanoidPlayerCharacter;

UENUM()

enum EConfectioniteBehaviour
{
	NONE,
	ANIMATION,
	LOOK_AT
};


UCLASS()
class SOULSGAME_API AConfectioniteCharacter : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConfectioniteCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTransform InitialBaseAuraTransform;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeAttachment(ASHumanoidPlayerCharacter * ParentCharacter);

	// void DoDonutBlast(const TFunction<void()> & OnFinished);
	// void DoDonutSpiral(const TFunction<void()> & OnFinished);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent * SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * OriginSceneComponent;
	
	UPROPERTY(EditAnywhere, Category="Confectionite")
	UChildActorComponent * FaceActorComponent;

	UPROPERTY(EditAnywhere, Category="Confectionite")
	UChildActorComponent * HitboxActorComponent;

	UPROPERTY(EditAnywhere)
	UProjectileShooterComponent * ProjectileSpawnerTest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent * BoxCollisionComponent;


	//void SetDebugMode(bool Set);


	ASHumanoidPlayerCharacter * GetAttachedCharacter() const;

	UPROPERTY(EditAnywhere)
	FVector ActorToConfectioniteOffset = FVector(90, 0, -90);

	EConfectioniteBehaviour ConfectioniteBehaviour;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// UProjectileShooterComponent * ProjectileShooterComponent;

	UPROPERTY(EditAnywhere)
	int32 ShotIndex = 0;

	AAbilityActor* GetHitboxActor() const;

	void ReattachToCharacter();
	void ReattachAfterSeconds(float Seconds, const TFunction<void()> & OnReattachConfectionite = nullptr);

	//void SetProjectileGameplayEffect(TWeakPtr<FGameplayEffectsWrapperContainer> Container);

	void DeactivatePS();

	// void Shoot();
	// void EndShoot();

	// Shoot location
	UFUNCTION(BlueprintCallable)
	FVector GetShootLocation() const;
	
protected:
	
	AActor * GetClosestTarget(float Radius);
	
	bool DebugMode = false;

	UPROPERTY()
	AActor * LookAtTarget;

	UPROPERTY()
	AActor * PreviousLookAtTarget = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface * RegularMaterial;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface * GlowMaterial;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent * BaseAuraPS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent * AuraPS;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraComponent * GlowPS;
	
	UPROPERTY()
	AAbilityActor* HitboxActor;

};
