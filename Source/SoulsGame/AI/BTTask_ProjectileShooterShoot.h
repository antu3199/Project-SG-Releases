// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ProjectileShooterShoot.generated.h"

/**
 * 
 */
class ASProjectileShooterCharacter;

UCLASS()
class SOULSGAME_API UBTTask_ProjectileShooterShoot : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ProjectileShooterShoot();

	UPROPERTY(EditAnywhere)
	bool bStartShooting = true;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void StartShooting(ASProjectileShooterCharacter * Character);
	void StopShooting(ASProjectileShooterCharacter * Character);
};
