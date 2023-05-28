// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ProjectileShooterShoot.h"


#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/SProjectileShooterCharacter.h"

UBTTask_ProjectileShooterShoot::UBTTask_ProjectileShooterShoot()
{
	NodeName = "Shoot";
}

EBTNodeResult::Type UBTTask_ProjectileShooterShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ASProjectileShooterCharacter * Character = Cast<ASProjectileShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}

	if (bStartShooting)
	{
		StartShooting(Character);
	}
	else
	{
		StopShooting(Character);
	}

	return EBTNodeResult::Succeeded;
	
}

void UBTTask_ProjectileShooterShoot::StartShooting(ASProjectileShooterCharacter* Character)
{
	if (!Character->IsShooting())
	{
		UProjectileShooterComponent * Spawner = Character->GetProjectileSpawner("PShooter_RotationalBarrage");
		APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn != nullptr)
		{
			Character->SetTarget(Spawner, PlayerPawn->GetRootComponent());
		}

		Character->StartShooting(Spawner);
	}
}

void UBTTask_ProjectileShooterShoot::StopShooting(ASProjectileShooterCharacter* Character)
{
	if (Character->IsShooting())
	{
		UProjectileShooterComponent * Spawner = Character->GetProjectileSpawner("PShooter_RotationalBarrage");
		Character->StopShooting(Spawner);
	}
}
