#pragma once
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "SGBTTask_Scoot.generated.h"


UCLASS()
class USGBTTask_Scoot : public UBTTask_MoveTo
{
    GENERATED_BODY()

    USGBTTask_Scoot();
protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

