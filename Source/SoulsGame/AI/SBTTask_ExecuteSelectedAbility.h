// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "SBTTask_ExecuteSelectedAbility.generated.h"


/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTTask_ExecuteSelectedAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USBTTask_ExecuteSelectedAbility();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;

	UPROPERTY(EditAnywhere)
	bool bResetAbility = true;

	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void EndTask();
	

	//UFUNCTION()
	//virtual void OnMontageCompleted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);

	UFUNCTION()
	void OnMontageFinished(class UMyGameplayAbility* Ability, FGameplayTag EventTag, FGameplayEventData EventPayload);

	bool bAbilityEnded = false;
	FDelegateHandle AbilityEndedHandle;
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp = nullptr;
	TWeakObjectPtr<class UMyGameplayAbility> CachedAbility;

	
};
