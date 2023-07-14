// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SGAIController.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API ASGAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	
public:

protected:

	private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior; 
};
