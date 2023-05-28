// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityActor.h"
#include "TimeStopAbilityActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API ATimeStopAbilityActor : public AAbilityActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeStopAbilityActor();

	virtual  void Initialize(UMyGameplayAbility* DataContainer);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual  void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual  void BeginPlay() override;

	virtual  void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TArray<AActor *> HitActors;

	bool IsOverlapping;

	UFUNCTION()
	void OnTagAdded(const FGameplayTag CooldownTag, int32 NewCount, ASCharacterBase * Actor);

	UFUNCTION()
	void OnTagRemoved(const FGameplayTag CooldownTag, int32 NewCount, ASCharacterBase * Actor);


	UPROPERTY(EditAnywhere)
	UMaterial * OutsideMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial * InsideMaterial;
};
