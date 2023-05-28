#include "SBTTask_Scoot.h"

USBTTask_Scoot::USBTTask_Scoot()
{
	NodeName = "Scoot";
}

EBTNodeResult::Type USBTTask_Scoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// TODO: Custom animation of some sort
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
