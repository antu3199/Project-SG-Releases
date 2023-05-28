#include "SBTService_Null.h"

USBTService_Null::USBTService_Null()
{
    NodeName = "Null";
}

void USBTService_Null::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}