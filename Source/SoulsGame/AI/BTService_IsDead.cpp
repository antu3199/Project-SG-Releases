// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsDead.h"

#include "AIController.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/SCharacterBase.h"

UBTService_IsDead::UBTService_IsDead()
{
	NodeName = "Set is dead";
}

void UBTService_IsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	ASCharacterBase * Character = Cast<ASCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());

	
	bool IsDead = Character->GetIsDead();

	ASCharacterBase * PlayerPawn = Cast<ASCharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerPawn != nullptr)
	{
		if (PlayerPawn->GetIsDead()) IsDead = true;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Is stunned? %d"), IsStunned);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), IsDead);
}
