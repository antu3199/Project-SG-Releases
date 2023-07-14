#include "SGBTTask_MoveToUsingAbility.h"

#include "AIController.h"
#include "SoulsGame/AI/SGAIHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SGCharacterBase.h"

USGBTTask_MoveToUsingAbility::USGBTTask_MoveToUsingAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Move To Using Ability";
}


UAITask_MoveTo* USGBTTask_MoveToUsingAbility::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp,
                                                              UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest)
{
	USGAIAbilityWrapper * AbilityWrapper = Cast<USGAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGBTTask_MoveToUsingAbility::PrepareMoveTask] Unable to find ability!"));
	}

	if (AbilityWrapper)
	{
		if (AbilityWrapper->MaximumActivationRange != -1.0f)
		{
			AcceptableRadius = AbilityWrapper->MaximumActivationRange;
		}
		else if (AbilityWrapper->MinimumActivationRange != -1.0f)
		{
			AcceptableRadius = AbilityWrapper->MaximumActivationRange;
		}
		else
		{
			AcceptableRadius = DefaultAcceptableRadius;
		}
		ObservedBlackboardValueTolerance = AcceptableRadius * 0.95f;
	}
	
	return Super::PrepareMoveTask(OwnerComp, ExistingTask, MoveRequest);
}
