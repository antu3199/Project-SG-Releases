// Fill out your copyright notice in the Description page of Project Settings.


#include "PShooter_RotationalBarrage.h"

#include "SoulsGame/Abilities/PowerUps/Components/Spawned/ProjectileActor.h"

UPShooter_RotationalBarrage::UPShooter_RotationalBarrage() : UProjectileShooterComponent()
 {
 }

void UPShooter_RotationalBarrage::StartShooting()
{
	Super::StartShooting();
 	DifferenceVec = RotEnd - RotStart;
 	//DifferenceVec.Y = DifferenceVec.Y;
 	
 	DoShot();
}


void UPShooter_RotationalBarrage::OnShotSpawn(AProjectileActor* SpawnedObject)
{
 	OnShotFinished.Broadcast();

 	// If looping, only stop if this stop is last one
 	if (bNextShotAsLast)
 	{
 		StopShooting();
 		return;
 	}

 	
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

 	/*
	FVector Direction = CurQuat * LocalShootDirection;
	Direction.Normalize();

	Helper_SetProjectileToDirection(SpawnedObject->GetProjectileMovement(), Direction);

*/
 	CurShot++;
 	T = (float)CurShot / (float)(NumShots - 1);
 	UpdateBarrageDirection(T);
 	

 	//ShootDirection = CurQuat * GetRelativeForward();

	
}

void UPShooter_RotationalBarrage::DoShot()
{
 	auto Callback = [&]()
 	{

 	};

 	AActor * Actor = GetOwner();

 	//FTransform Transform = Actor->GetActorTransform();
 	//ShootDirection = GetRelativeRight();
 	
 	//ShootDirection = FVector::UpVector;
 	//UpdateBarrageDirection(0);
 	ShootDirection = GetInitialBarrageDirection();
	CurShot = 0;
	DoBarrage(TimeBetweenShots, NumShots);
}

void UPShooter_RotationalBarrage::OnEndBarrage()
{
 	Super::OnEndBarrage();
 	OnShootingFinished.Broadcast();
 	if (!bPlayLooping)
 	{
 		IsShooting = false;
 	}
}

FVector UPShooter_RotationalBarrage::GetInitialBarrageDirection() const
{
 	FVector InitialDirection = GetRelativeCharacterForward();

 	FVector Direction = InitialDirection;
 	Direction = Direction.RotateAngleAxis(RotStart.X, GetRelativeCharacterForward());
 	Direction = Direction.RotateAngleAxis(-RotStart.Y, GetRelativeCharacterRight());  // Probably can be fixed by modifying XYZ rotation order, or hardcode it like this instead :)
 	Direction = Direction.RotateAngleAxis(RotStart.Z, GetRelativeCharacterUp());
 	return Direction;
}

void UPShooter_RotationalBarrage::UpdateBarrageDirection(float Tick)
{
 	FVector InitialDirection = GetInitialBarrageDirection();

 	ShootDirection = InitialDirection;
 	ShootDirection = ShootDirection.RotateAngleAxis(DifferenceVec.X * Tick, GetRelativeCharacterForward());
 	ShootDirection = ShootDirection.RotateAngleAxis(DifferenceVec.Y * Tick, GetRelativeCharacterRight());
 	ShootDirection = ShootDirection.RotateAngleAxis(DifferenceVec.Z * Tick, GetRelativeCharacterUp());
}

void UPShooter_RotationalBarrage::StopShooting()
{
 	Super::StopShooting();
 	CurShot = 0;
}

bool UPShooter_RotationalBarrage::PreShot()
{
	// ShootDirection += GetRelativeForward();
 	// ShootDirection.Normalize();
 	
 	OnPreShot.Broadcast();
 	return true;
}
