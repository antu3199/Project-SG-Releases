// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "SGBTTask_ExecuteSelectedAbility.generated.h"


/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTTask_ExecuteSelectedAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USGBTTask_ExecuteSelectedAbility();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;

	UPROPERTY(EditAnywhere)
	bool bResetAbility = true;

	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void EndTask();
	

	//UFUNCTION()
	//virtual void OnMontageCompleted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);

	UFUNCTION()
	void OnMontageFinished(int32 InstanceHandle);

	bool bAbilityEnded = false;
	FDelegateHandle AbilityEndedHandle;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp = nullptr;
	TWeakObjectPtr<class USGAbility> CachedAbility;
	int32 CachedAbilityHandle = INDEX_NONE;

	
};
