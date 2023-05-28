#pragma once
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "SBTTask_Scoot.generated.h"


UCLASS()
class USBTTask_Scoot : public UBTTask_MoveTo
{
    GENERATED_BODY()

    USBTTask_Scoot();
protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

