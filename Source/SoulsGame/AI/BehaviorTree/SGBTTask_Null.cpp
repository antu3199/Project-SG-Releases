// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTTask_Null.h"


#include "AIController.h"
#include "SoulsGame/Character/SGHumanoidCharacter.h"

USGBTTask_Null::USGBTTask_Null()
{
	NodeName = "Null";
}

EBTNodeResult::Type USGBTTask_Null::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	return EBTNodeResult::Succeeded;
	
}
