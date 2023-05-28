// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "SBTTask_ExecuteSelectedAbility_Legacy.generated.h"

struct FBTExecuteSelectedAbilityMemory_Legacy
{
	bool bAbilityEnded = false;
	FDelegateHandle AbilityEndedHandle;
	UBehaviorTreeComponent * CachedOwnerComp = nullptr;
	FGameplayAbilitySpecHandle AbilitySpecHandle;
};


/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTTask_ExecuteSelectedAbility_Legacy : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USBTTask_ExecuteSelectedAbility_Legacy();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;


	UPROPERTY(EditAnywhere)
	bool bResetAbility = true;

	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	uint16 GetInstanceMemorySize() const override;
	
	uint8* CachedNodeMemory;
};
