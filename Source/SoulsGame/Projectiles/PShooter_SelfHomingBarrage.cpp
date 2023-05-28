// Fill out your copyright notice in the Description page of Project Settings.


#include "PShooter_SelfHomingBarrage.h"

#include "SoulsGame/Abilities/PowerUps/Components/Spawned/ProjectileActor.h"

UPShooter_SelfHomingBarrage::UPShooter_SelfHomingBarrage() : UPShooter_RotationalBarrage()
{
}

void UPShooter_SelfHomingBarrage::OnEndBarrage()
{
	Super::OnEndBarrage();
	OnShotFinished.Broadcast();
	if (!bPlayLooping)
	{
		IsShooting = false;
	}
}

void UPShooter_SelfHomingBarrage::OnShotSpawn(AProjectileActor* SpawnedObject)
{
	Super::OnShotSpawn(SpawnedObject);

	UProjectileMovementComponent * ProjectileMovement = SpawnedObject->GetProjectileMovement();
	if (!ProjectileMovement)
	{
		return;
	}

	const FVector CurRotation = FMath::Lerp(RotStart, RotEnd, T);
	const FQuat CurQuat = FQuat::MakeFromEuler(CurRotation);
	//const FQuat StartQuat = FQuat::MakeFromEuler(RotStart);
	//const FQuat EndQuat = FQuat::MakeFromEuler(RotEnd);
	//const FQuat CurQuat = FQuat::SlerpFullPath(StartQuat, EndQuat, T);
	
	
	FVector Direction = CurQuat * LocalShootDirection;
	Direction.Normalize();

	FVector WorldDirection = ProjectileMovement->UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(Direction);
	WorldDirection.Normalize();
	TransformOffset = WorldDirection * TransformOffsetMagnitude; //ProjectileMovement->UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(Direction) * TransformOffsetMagnitude;


	const FVector ModifiedRotation = CurRotation + ModifiedRot;

	const FQuat ModifiedQuat = FQuat::MakeFromEuler(ModifiedRotation);

	Direction = ModifiedQuat * LocalShootDirection;
	Direction.Normalize();

	Helper_SetProjectileToDirection(SpawnedObject->GetProjectileMovement(), Direction);

	CurShot++;
	T = (float)CurShot / (float)(NumShots - 1);
}


void UPShooter_SelfHomingBarrage::DoShot()
{
	
	CurShot = 0;
	Target = GetOwner()->GetRootComponent();
	DoBarrage(TimeBetweenShots, NumShots);
}
