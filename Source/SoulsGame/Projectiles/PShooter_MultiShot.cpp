// Fill out your copyright notice in the Description page of Project Settings.


#include "PShooter_MultiShot.h"

UPShooter_MultiShot::UPShooter_MultiShot() : Super()
{
}

void UPShooter_MultiShot::StartShooting()
{
	Super::StartShooting();
}

void UPShooter_MultiShot::StopShooting()
{
	Super::StopShooting();
	// I think base is fine
}

void UPShooter_MultiShot::DoShot()
{
	// Super::DoShot();
	// Base is fine

	if (PreShot())
	{

		if (CurrentLevel >= 0)
		{
			SingleShot(ShootDirection);
		}

/*		
		if (CurrentLevel >= 1)
		{
			const FVector Left = ShootDirection.RotateAngleAxis(-45, GetCameraRelativeUp());
			const FVector Right = ShootDirection.RotateAngleAxis(45, GetCameraRelativeUp());
			SingleShot(Right);
			SingleShot(Left);
		
		}
		*/

		UE_LOG(LogTemp, Warning, TEXT("Current Level %d"), CurrentLevel);

		const int32 MaxLevelPerRow = 4;

		int32 RunningLevel = 0;
		int32 CurrentVerticalLevel = 0;

		const int32 VerticalDegrees = 7.5f;
		// Temp:
		//CurrentVerticalLevel = 1;
		while (RunningLevel <= CurrentLevel)
		{
			FVector ShotVector = ShootDirection;

			if (CurrentVerticalLevel % 2 == 1)
			{
				// Upwards
				ShotVector = ShotVector.RotateAngleAxis(-VerticalDegrees * CurrentVerticalLevel, GetCameraRelativeRight());
			}
			else
			{
				// Downards
				ShotVector = ShotVector.RotateAngleAxis(VerticalDegrees * CurrentVerticalLevel, GetCameraRelativeRight());
			}

			float CurDeg = 45;
			for (int32 i = 1; i <= MaxLevelPerRow; i++)
			{
				RunningLevel++;
				if (RunningLevel > CurrentLevel)
				{
					break;
				}
				
				if (i % 2 == 1)
				{
					const FVector Right = ShotVector.RotateAngleAxis(CurDeg, GetCameraRelativeUp());
					SingleShot(Right);
				}
				else
				{
					const FVector Left = ShotVector.RotateAngleAxis(-CurDeg, GetCameraRelativeUp());
					SingleShot(Left);
					CurDeg /= 2;
				}
			}

			CurrentVerticalLevel++;
		}
		

		
		/*
		float CurDeg = 45;
		int32 NumIterations = CurrentLevel * 2;

		
		
		for (int32 i = 1; i <= CurrentLevel; i++)
		{
			if (i % 2 == 1)
			{
				const FVector Right = ShootDirection.RotateAngleAxis(CurDeg, GetCameraRelativeUp());
				SingleShot(Right);
			}
			else
			{
				const FVector Left = ShootDirection.RotateAngleAxis(-CurDeg, GetCameraRelativeUp());
				SingleShot(Left);
				CurDeg /= 2;
			}
			
		}

*/
		OnShotFinished.Broadcast();

		if (!bPlayLooping)
		{
			// If not looping, then stop immedately
			StopShooting();
		}
		else
		{
			// If looping, only stop if this stop is last one
			if (bNextShotAsLast)
			{
				StopShooting();
			}
		}
	}
}

void UPShooter_MultiShot::OnShotSpawn(AProjectileActor* SpawnedObject)
{
	// Moved to DoShot()

	/*
	Super::OnShotSpawn(SpawnedObject);

	if (!bPlayLooping)
	{
		// If not looping, then stop immedately
		StopShooting();
	}
	else
	{
		// If looping, only stop if this stop is last one
		if (bNextShotAsLast)
		{
			StopShooting();
		}
	}
	*/
}

void UPShooter_MultiShot::OnTargetUpdated(const FVector& Direction)
{
	Super::OnTargetUpdated(Direction);
}

