// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTService_CanMove.h"


#include "AIController.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"

USGBTService_CanMove::USGBTService_CanMove()
{
	NodeName = "Set CanMove";
}

void USGBTService_CanMove::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	ASGCharacterBase * Character = Cast<ASGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());


	bool CanMove = true;
	CanMove &= Character->GetCanMove();

	//const FGameplayTag StunTag = USGAbilitySystemGlobals::GetSG().Tag_State_Stun;

	if (USGAbilityComponent* AbilityComponent = Character->FindComponentByClass<USGAbilityComponent>())
	{
		// if (AbilityComponent->HasActiveEffect(StunTag))
		// {
		// 	CanMove = false;
		// }
	}

	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), CanMove);

}
