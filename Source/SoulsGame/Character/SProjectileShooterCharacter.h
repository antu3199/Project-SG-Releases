// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Projectiles/ISShooter.h"
#include "SProjectileShooterCharacter.generated.h"


// Note: IAbilitySystemInterface needed otherwise it won't listen to gameplay events
UCLASS(Blueprintable)
class SOULSGAME_API ASProjectileShooterCharacter : public ASCharacterBase, public ISShooter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASProjectileShooterCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	class TSubclassOf<class USShootAbility> DebugGameplayAbility;
	
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	FString ProjectileSpawnerParentSocket;

	virtual FVector GetDefaultTarget() const override;
	virtual USceneComponent* GetDefaultHomingTarget() const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;

	//FGameplayAbilitySpecWrapper* CachedAbilitySpecWrapper;

	UPROPERTY()
	USceneComponent * OriginSceneComponent;

	
};

