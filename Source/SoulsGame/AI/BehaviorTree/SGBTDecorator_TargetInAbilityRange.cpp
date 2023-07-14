#include "SGBTDecorator_TargetInAbilityRange.h"

#include "AIController.h"
#include "SoulsGame/AI/SGAIHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SGCharacterBase.h"

USGBTDecorator_TargetInAbilityRange::USGBTDecorator_TargetInAbilityRange()
{
    NodeName = "TargetInAbilityRange?";
}


bool USGBTDecorator_TargetInAbilityRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	/*
	UAIAbilityWrapper_Legacy* AbilityWrapper = Cast<UAIAbilityWrapper_Legacy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (AbilityWrapper == nullptr)
	{
		return false;
	}

	float Radius = AbilityWrapper->MoveToAcceptableRadius;

	if (Radius == -1.0f)
	{
		return true;
	}
	
	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return false;
	}

	AActor * Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	ASCharacterBase * Character = Cast<ASCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return false;
	}
	float Distance = FVector::Distance(Target->GetActorLocation(), Character->GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("Range: Distance: %f Ability Radius: %f "), Distance, Radius);

	if (Distance <= Radius)
	{
		return true;
	}
*/
	
	return false;
}
