// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SGBTService_Null.generated.h"

/**
* 
*/
UCLASS()
class SOULSGAME_API USGBTService_Null : public UBTService_BlackboardBase
{
    GENERATED_BODY()
    public:
    USGBTService_Null();

    protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
