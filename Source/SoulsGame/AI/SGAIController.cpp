// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAIController.h"


void ASGAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASGAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASGAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
	}
}
