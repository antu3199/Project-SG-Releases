// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageAbility.h"
#include "PlayMontageCharacterAbility.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DashUntilReachTargetAbility.generated.h"

/**
 * 
 */
// PlayMontage for specifically Humanoid characters 
UCLASS()
class SOULSGAME_API UDashUntilReachTargetAbility : public UPlayMontageCharacterAbility
{
	GENERATED_BODY()

public:
	void SetStoppingDistance(float Distance);

	virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:

	virtual  void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	bool UnFreezeCondition();

	void OnCharacterHitObject(ASCharacterBase* Character, AActor* Other, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float StoppingDistance = 150.0f;
	
	UPROPERTY(Transient)
	class UBlackboardComponent* BlackboardComponent = nullptr;

	UPROPERTY(Transient)
	class AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FName TargetActorBlackboardKey;

	bool bStopDueToHitActor = false;
	float CachedLastDistance = -1.0f;
};



