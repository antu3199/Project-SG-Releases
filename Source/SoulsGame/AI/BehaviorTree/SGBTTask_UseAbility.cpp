// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTTask_UseAbility.h"


#include "AIController.h"
#include "SoulsGame/Character/SGHumanoidCharacter.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"

USGBTTask_UseAbility::USGBTTask_UseAbility()
{
	NodeName = "Attack";
}
// TODO: Remove me!
EBTNodeResult::Type USGBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ASGHumanoidCharacter * Character = Cast<ASGHumanoidCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}

	if (USGAbilityComponent* AbilityComponent = Character->FindComponentByClass<USGAbilityComponent>())
	{
		AbilityComponent->RequestActivateAbilityByTag(Tag);
	}
	
	return EBTNodeResult::Succeeded;
	
}
