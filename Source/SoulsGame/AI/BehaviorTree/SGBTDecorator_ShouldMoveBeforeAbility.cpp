#include "SGBTDecorator_ShouldMoveBeforeAbility.h"

#include "SoulsGame/AI/SGAIHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

USGBTDecorator_ShouldMoveBeforeAbility::USGBTDecorator_ShouldMoveBeforeAbility()
{
	NodeName = "ShouldMoveBeforeAbility?";
}

bool USGBTDecorator_ShouldMoveBeforeAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	USGAIAbilityWrapper * AbilityWrapper = Cast<USGAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		return false;
	}
	
	return AbilityWrapper->bDoWalk;
}
