// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTService_IsDead.h"

#include "AIController.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/SGCharacterBase.h"

USGBTService_IsDead::USGBTService_IsDead()
{
	NodeName = "Set is dead";
}

void USGBTService_IsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	ASGCharacterBase * Character = Cast<ASGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());

	
	bool IsDead = Character->GetIsDead();

	ASGCharacterBase * PlayerPawn = Cast<ASGCharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerPawn != nullptr)
	{
		if (PlayerPawn->GetIsDead()) IsDead = true;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Is stunned? %d"), IsStunned);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), IsDead);
}
