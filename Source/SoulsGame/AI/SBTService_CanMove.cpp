// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTService_CanMove.h"


#include "AIController.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"

USBTService_CanMove::USBTService_CanMove()
{
	NodeName = "Set CanMove";
}

void USBTService_CanMove::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	ASCharacterBase * Character = Cast<ASCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());


	bool CanMove = true;
	CanMove &= Character->GetCanMove();

	const FGameplayTag StunTag = USGAbilitySystemGlobals::GetSG().Tag_State_Stun;
	if (Character->GetAbilitySystemComponent()->HasMatchingGameplayTag(StunTag))
	{
		CanMove = false;
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), CanMove);

}
