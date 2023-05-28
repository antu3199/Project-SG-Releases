#include "SGMarkerController.h"

#include "SGMarkerActor.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGameInstance.h"
#include "SoulsGame/SUtils.h"

FMarkerInfo::FMarkerInfo()
{
}

FMarkerInfo::FMarkerInfo(int32 InMarkerId)
{
	MarkerId = InMarkerId;
}

USGMarkerController* USGMarkerController::GetSingleton(const UObject* WorldContextObject)
{
	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		return GameInstance->GetMarkerController();
	}

	return nullptr;
}

void USGMarkerController::Initialize()
{
}

int32 USGMarkerController::CreateNewMarker(FVector MarkerLocation, FVector2f Size, bool bCalculateGroundPosition)
{
	FVector GroundLocation;

	if (bCalculateGroundPosition)
	{
		const bool bIsValidLocation = FSUtils::GetGroundLocation(GetWorld(), MarkerLocation, GroundLocation);
		if (!bIsValidLocation)
		{
			return -1;
		}
	}
	else
	{
		GroundLocation = MarkerLocation;
	}

	
	int32 MarkerId = INVALID_MARKER_ID;
	if (!ReusableMarkerIds.IsEmpty())
	{
		int32 Id = *ReusableMarkerIds.begin();
		MarkerId = Id;
		ReusableMarkerIds.Remove(MarkerId);
	}
	else
	{
		MarkerId = RunningMarkerId++;
	}

	MarkerObjects.Add(MarkerId, FMarkerInfo(MarkerId));

	FMarkerInfo& MarkerInfo = MarkerObjects[MarkerId];
	MarkerInfo.World = MakeWeakObjectPtr(GetWorld());


	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Default
	SpawnParameters.bNoFail = true;
	SpawnParameters.Instigator = nullptr;
	ASGMarkerActor* SpawnedObject = GetWorld()->SpawnActor<ASGMarkerActor>(MarkerActorTemplate, GroundLocation, FRotator::ZeroRotator, SpawnParameters);
	if (!SpawnedObject)
	{
		return INVALID_MARKER_ID;
	}

	SpawnedObject->SetDecalSize(Size.X, Size.Y);
	MarkerInfo.MarkerActor = SpawnedObject;

	return MarkerId;
}

void USGMarkerController::RemoveMarker(int32 MarkerId)
{
	if (MarkerObjects.Contains(MarkerId))
	{
		FMarkerInfo& MarkerInfo = MarkerObjects[MarkerId];

		if (IsValid(MarkerInfo.MarkerActor))
		{
			MarkerInfo.MarkerActor->RemoveMarker();
		}

		MarkerObjects.Remove(MarkerId);
		ReusableMarkerIds.Add(MarkerId);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to remove marker %d"), MarkerId);
	}
}


