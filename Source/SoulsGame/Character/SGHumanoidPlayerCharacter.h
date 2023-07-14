// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGHumanoidCharacter.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "SoulsGame/DataAssets/SGWeaponAsset.h"
#include "GameFramework/Character.h"
#include "SoulsGame/Weapon/SGWeaponActor.h"

#include "SGHumanoidPlayerCharacter.generated.h"

class USGCharacterMovementComponent;
class USGAnimNotifyState_JumpSection;

// Hint for special cases when queuing abilities
enum ECharacterActionHint
{
	Hint_Default = 0,
	Hint_BasicAttack = 1
};

// Note: IAbilitySystemInterface needed otherwise it won't listen to gameplay events
UCLASS(Blueprintable)
class SOULSGAME_API ASGHumanoidPlayerCharacter : public ASGHumanoidCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASGHumanoidPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	class USGCharacterInventoryComponent* GetInventoryComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class USGCharacterInventoryComponent* CharacterInventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	class  USGSlotComponent* SlotComponent;
	
};
