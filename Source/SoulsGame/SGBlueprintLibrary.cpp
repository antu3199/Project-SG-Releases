#include "SGBlueprintLibrary.h"

#include "SGGameInstance.h"
#include "SGGameSingleton.h"
#include "Kismet/GameplayStatics.h"


float SGBlueprintLibrary::GetDistanceFromCamera(const UObject* WorldContextObject, int32 PlayerIndex, const FVector& Location)
{
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(WorldContextObject, PlayerIndex))
	{
		const FVector CameraLocation = Pawn->GetController<APlayerController>()->PlayerCameraManager->GetCameraLocation();
		return FVector::Distance(CameraLocation, Location);
	}

	return 0;
}

