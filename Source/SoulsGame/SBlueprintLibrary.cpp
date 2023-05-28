#include "SBlueprintLibrary.h"

#include "SGameInstance.h"
#include "SGameSingleton.h"
#include "Kismet/GameplayStatics.h"

USHitboxManager* SBlueprintLibrary::GetHitboxManager()
{
	if (USGameSingleton * GameSingleton = Cast<USGameSingleton>(GEngine->GameSingleton))
	{
		return GameSingleton->GetHitboxManager();
	}
	
	return nullptr;
}

float SBlueprintLibrary::GetDistanceFromCamera(const UObject* WorldContextObject, int32 PlayerIndex, const FVector& Location)
{
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(WorldContextObject, PlayerIndex))
	{
		const FVector CameraLocation = Pawn->GetController<APlayerController>()->PlayerCameraManager->GetCameraLocation();
		return FVector::Distance(CameraLocation, Location);
	}

	return 0;
}

