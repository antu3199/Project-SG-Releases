// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_SetScoot.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"
#include "SoulsGame/SUtils.h"

USBTTask_SetScoot::USBTTask_SetScoot()
{
	NodeName = "SetScoot";
}

EBTNodeResult::Type USBTTask_SetScoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const float ScootChance = ChanceToScoot;
	float ShouldDoScoot = FMath::RandRange(0.0, 1.0);
	if (ShouldDoScoot >= (1 - ScootChance))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(DoScootBlackboardKey.SelectedKeyName, true);
		return DoScoot(OwnerComp, NodeMemory);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(DoScootBlackboardKey.SelectedKeyName, false);
	}
	
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type USBTTask_SetScoot::DoScoot(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASCharacterBase * Character = Cast<ASCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}

	int32 MovementChoice = FMath::RandRange(0, 2);
	FVector MovementDirection;
	switch (MovementChoice)
	{
	case 0:
		MovementDirection = FVector::BackwardVector;
		break;
	case 1:
		MovementDirection = FVector::RightVector;
		break;
	case 2:
		MovementDirection = FVector::LeftVector;
		break;
	default:
		MovementDirection = FVector::BackwardVector;
	}

	const float ScootDistance = FMath::RandRange(MinScootDistance, MaxScootDistance);
	const FVector ScootLocation = Character->GetTransform().TransformPosition(MovementDirection * ScootDistance);
	const float Radius = 50;

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(ScootLocationBlackboardKey.SelectedKeyName, ScootLocation);

	return EBTNodeResult::Succeeded;
}
