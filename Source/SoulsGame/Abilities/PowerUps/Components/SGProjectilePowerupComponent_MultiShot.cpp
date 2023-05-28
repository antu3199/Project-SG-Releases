#include "SGProjectilePowerupComponent_MultiShot.h"

USGProjectilePowerupComponent_MultiShot::USGProjectilePowerupComponent_MultiShot() : Super()
{
}

void USGProjectilePowerupComponent_MultiShot::DoPowerup()
{
	Super::DoPowerup();

	// Super::DoShot();
	// Base is fine



	if (CurrentLevel >= 0)
	{
		SingleShot(ShootDirection);
	}

	UE_LOG(LogTemp, Warning, TEXT("Current Level %d"), CurrentLevel);

	/*
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
	*/
	int32 MaxLevel = 5;
	int32 UsedLevel = FMath::Min(MaxLevel, CurrentLevel);
	float CurDeg = 45;
	for (int32 i = 1; i <= UsedLevel; i++)
	{
		FVector ShotVector = ShootDirection;

		const FVector Right = ShotVector.RotateAngleAxis(CurDeg, GetCameraRelativeUp());
		SingleShot(Right);
		const FVector Left = ShotVector.RotateAngleAxis(-CurDeg, GetCameraRelativeUp());
		SingleShot(Left);
		CurDeg /= 2;
	}

	OnPostDoPowerup.Broadcast();

	if (!bPlayLooping)
	{
		// If not looping, then stop immedately
		StopPowerup();
	}
	else
	{
		// If looping, only stop if this stop is last one
		if (bNextShotAsLast)
		{
			StopPowerup();
		}
	}
}

void USGProjectilePowerupComponent_MultiShot::OnShotSpawn(AProjectileActor* SpawnedObject)
{
	// Moved to DoShot()
}

