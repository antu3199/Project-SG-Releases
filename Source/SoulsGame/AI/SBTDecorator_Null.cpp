#include "SBTDecorator_Null.h"

USBTDecorator_Null::USBTDecorator_Null()
{
    NodeName = "Null";
}

void USBTDecorator_Null::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}
