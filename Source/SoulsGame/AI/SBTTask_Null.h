// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_Null.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTTask_Null : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USBTTask_Null();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
