// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_Null.h"


#include "AIController.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"

USBTTask_Null::USBTTask_Null()
{
	NodeName = "Null";
}

EBTNodeResult::Type USBTTask_Null::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	return EBTNodeResult::Succeeded;
	
}
