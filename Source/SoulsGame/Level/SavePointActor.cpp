#include "SavePointActor.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameModeBase.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/SaveLoad/SGSaveLoadManager.h"

ASavePointActor::ASavePointActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	PlayerSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerSpawnPoint"));
	if (PlayerSpawnPoint)
	{
		PlayerSpawnPoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		PlayerSpawnPoint->SetRelativeLocation(FVector(-100.0f, 0.0f, 0.0f));
	}
}

void ASavePointActor::OnSaveGame(FSGSaveRecord& SaveRecord)
{
	int32 Test = 0;
}

void ASavePointActor::LoadGame(const FSGSaveRecord& LoadRecord)
{
	int32 Test = 0;

}

void ASavePointActor::BeginPlay()
{
	Super::BeginPlay();

	bCurrentVisibility = IsVisible();

	SetVisibility(bCurrentVisibility);
}

FSavePointTransformData ASavePointActor::GetSavepointTransformData() const
{
	FSavePointTransformData Data;
	Data.Location = GetSpawnPoint();
	Data.Rotation = GetSpawnRotation();
	if (GetLevel() != nullptr && GetLevel()->GetOuter() != nullptr)
	{
		Data.PathName = GetLevel()->GetOuter()->GetFName();
	}
	return Data;
}

void ASavePointActor::CheckUnlockVisibility()
{
	if (IsVisible())
	{
		SetVisibility(true);
	}
}

void ASavePointActor::SetVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		FSGUtils::SetActorEnabled(this, true);
		OnUnlockVisibility();
	}
	else
	{
		FSGUtils::SetActorEnabled(this, false);
	}
}

bool ASavePointActor::IsVisible() const
{
	if (VisiblityRequirements.IsEmpty())
	{
		return true;
	}
	
	if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		return GameInstance->GetSaveLoadManager()->GetCurrentSaveRecord().LevelProgressionData.HasAll(VisiblityRequirements);
	}

	return true;
}


void ASavePointActor::OnSavePointPressed()
{
}

FVector ASavePointActor::GetSpawnPoint() const
{
	return PlayerSpawnPoint->GetComponentLocation();
}

FRotator ASavePointActor::GetSpawnRotation() const
{
	return PlayerSpawnPoint->GetComponentRotation();
}

bool ASavePointActor::IsUnlocked() const
{
	if (!SavePointTag.IsValid())
	{
		return false;	
	}
	
	if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		return GameInstance->GetSaveLoadManager()->GetCurrentSaveRecord().UnlockedSavePoints.HasTag(SavePointTag);
	}

	return false;
}

void ASavePointActor::SetUnlocked()
{
	if (!SavePointTag.IsValid())
	{
		return;	
	}

	if (IsUnlocked())
	{
		return;
	}

	if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GameInstance->GetSaveLoadManager()->GetCurrentSaveRecordMutable().UnlockedSavePoints.AddTag(SavePointTag);
	}
}


void ASavePointActor::OnInteract(ASGPlayerController* Controller)
{
	SetUnlocked();
	if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->RestartGameAtSavePoint(SavePointTag, true);
	}

	//OpenSaveUI(Controller);
}

