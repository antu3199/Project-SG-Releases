// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SHumanoidCharacter.h"
#include "AbilitySystemInterface.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"
#include "SoulsGame/Abilities/Attributes/MyAttributeSet.h"
#include "SoulsGame/Character/ConfectioniteCharacter.h"
#include "Components/TimelineComponent.h"
#include "SoulsGame/DataAssets/WeaponAsset.h"
#include "GameFramework/Character.h"
#include "SoulsGame/Weapon/WeaponActor.h"
#include "SoulsGame/Projectiles/ISShooter.h"

#include "SHumanoidPlayerCharacter.generated.h"

class UMyCharacterMovementComponent;
class UJumpSectionNS;

// Hint for special cases when queuing abilities
enum ECharacterActionHint
{
	Hint_Default = 0,
	Hint_BasicAttack = 1
};

// Note: IAbilitySystemInterface needed otherwise it won't listen to gameplay events
UCLASS(Blueprintable)
class SOULSGAME_API ASHumanoidPlayerCharacter : public ASHumanoidCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASHumanoidPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginShooting();
	void EndShooting();
	bool GetIsShooting() const;

	UFUNCTION(BlueprintCallable)
	class USGCharacterInventoryComponent* GetInventoryComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsShooting = false;

	UPROPERTY(EditDefaultsOnly)
	class USGCharacterInventoryComponent* CharacterInventoryComponent;
};
