// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "SoulsGame/Abilities/GameplayEffectActor.h"

#include "WeaponActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAttackBegan);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAttackEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoWeaponAttackBeganVisuals);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoWeaponAttackEndVisuals);

class UCapsuleComponent;
UCLASS()
class SOULSGAME_API AWeaponActor : public AGameplayEffectActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

	virtual void PostInitProperties() override;

	virtual  void BeginDestroy() override;
	virtual void TickActor( float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction ) override;

	bool CheckEmpowerWeapon();
	void DisableTimestopIfEmpoweredWeapon();
	void DisableEmpowerWeapon();

	void BroadcastWeaponBeginVisuals();
	void BroadcastWeaponEndVisuals();

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttackBegan OnWeaponAttackBegan;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttackEnd OnWeaponAttackEnd;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttackBegan OnWeaponAttackBeganVisuals;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttackEnd OnWeaponAttackEndVisuals;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CanSpawnFireActor() const;

	//FGameplayTag AttackEventTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * FXSpawnLocation;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> FireTemplate;

	UPROPERTY(EditAnywhere)
	float EmpoweredTimeForSpawningFire = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SwingFXOffset;

	// Note: Unfortunately, you cannot set niagara component custom parameters if done in C++ which is why this is commented out
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// UNiagaraComponent* WeaponSwingFX;
	
	//bool CanHit = true;
	FTimerHandle CanHitTimer;

	void SetCanHitTimer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent * SkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent * CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;
	
	UPROPERTY(Transient)
	UNiagaraComponent* FX_P_FlameSword;
	
	void BeginWeaponAttack();

	void EndWeaponAttack();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY()
	TSet<AActor*> HitActors;

	bool IsAttacking = false;

	int32 HitboxId = -1;
	bool bIsUsingEmpoweredAttack = false;
	bool bTimestoppedByEmpoweredAttack  = false;
	float EmpoweredTimer = 0;
};
