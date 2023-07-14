// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SGBTTask_UseAbility.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTTask_UseAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USGBTTask_UseAbility();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;
};
