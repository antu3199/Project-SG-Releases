// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "SGBTDecorator_Null.generated.h"

/**
* 
*/
UCLASS()
class SOULSGAME_API USGBTDecorator_Null : public UBTDecorator_BlackboardBase
{
    GENERATED_BODY()
    public:
    USGBTDecorator_Null();

    protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
