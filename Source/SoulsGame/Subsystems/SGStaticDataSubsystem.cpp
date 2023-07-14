#include "SGStaticDataSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameInstance.h"

void USGStaticDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USGStaticDataSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UStaticSGData* USGStaticDataSubsystem::GetStaticSGData() const
{
	if (const USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		if (UStaticSGData* StaticSGData = GameInstance->GetStaticDataMutable())
		{
			return StaticSGData;
		}
	}

	return nullptr;
}
