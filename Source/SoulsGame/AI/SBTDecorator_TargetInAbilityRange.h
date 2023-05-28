// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "SBTDecorator_TargetInAbilityRange.generated.h"

/**
* 
*/
UCLASS()
class SOULSGAME_API USBTDecorator_TargetInAbilityRange : public UBTDecorator_BlackboardBase
{
    GENERATED_BODY()
    public:
    USBTDecorator_TargetInAbilityRange();

    protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    UPROPERTY(EditAnywhere, Category=Blackboard)
    struct FBlackboardKeySelector AbilityHandleBlackboardKey;

    UPROPERTY(EditAnywhere, Category=Blackboard)
    struct FBlackboardKeySelector TargetActorBlackboardKey;
};
