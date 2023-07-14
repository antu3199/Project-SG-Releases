// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "SGBTDecorator_ShouldMoveBeforeAbility.generated.h"

/**
* 
*/
UCLASS()
class SOULSGAME_API USGBTDecorator_ShouldMoveBeforeAbility : public UBTDecorator_BlackboardBase
{
    GENERATED_BODY()
public:
    USGBTDecorator_ShouldMoveBeforeAbility();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    UPROPERTY(EditAnywhere, Category=Blackboard)
    struct FBlackboardKeySelector AbilityHandleBlackboardKey;
};
