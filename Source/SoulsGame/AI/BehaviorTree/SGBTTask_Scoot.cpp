#include "SGBTTask_Scoot.h"

USGBTTask_Scoot::USGBTTask_Scoot()
{
	NodeName = "Scoot";
}

EBTNodeResult::Type USGBTTask_Scoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// TODO: Custom animation of some sort
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
