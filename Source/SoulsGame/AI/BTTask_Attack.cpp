// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"


#include "AIController.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ASHumanoidCharacter * Character = Cast<ASHumanoidCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	
	if (!Character->IsAttacking())
	{
		Character->UseAbility("Ability.Melee");
	}
	
	return EBTNodeResult::Succeeded;
	
}
