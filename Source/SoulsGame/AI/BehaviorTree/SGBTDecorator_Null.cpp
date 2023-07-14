#include "SGBTDecorator_Null.h"

USGBTDecorator_Null::USGBTDecorator_Null()
{
    NodeName = "Null";
}

void USGBTDecorator_Null::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}
