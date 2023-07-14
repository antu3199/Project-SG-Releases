#include "SGSavePointMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/Level/SavePointActor.h"
#include "SoulsGame/SaveLoad/SGSaveLoadManager.h"

USGSavePointMenuWidget::USGSavePointMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGSavePointMenuWidget::CustomInitialize()
{
}

void USGSavePointMenuWidget::DoQuickTravel()
{
}

void USGSavePointMenuWidget::DoSave()
{
}

void USGSavePointMenuWidget::DoLoad()
{
}

void USGSavePointMenuWidget::DoReturnToGame()
{
}

void USGSavePointMenuWidget::ExitToMainMenu()
{
}

void USGSavePointMenuWidget::Debug_UnlockAll()
{
	if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		const UStaticSGData* StaticData = GameInstance->GetStaticData();
		for (const FSavePointData& SavePointData : StaticData->SavePointData)
		{
			GameInstance->GetSaveLoadManager()->GetCurrentSaveRecordMutable().UnlockedSavePoints.AddTag(SavePointData.Tag);
		}
	}
}

TArray<USavePointEntryData*> USGSavePointMenuWidget::GetSavePointEntryData() const
{
	TArray<USavePointEntryData*> Result;
	if (USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		// TODO: Enable this
		const FSGSaveRecord& SaveRecord = GameInstance->GetSaveLoadManager()->GetCurrentSaveRecord();
			
		//return GameInstance->GetSaveLoadManager()->GetCurrentSaveRecord().UnlockedSavePoints.HasTag(SavePointTag);
		const UStaticSGData* StaticData = GameInstance->GetStaticData();
		for (const FSavePointData& SavePointData : StaticData->SavePointData)
		{
			if (SaveRecord.UnlockedSavePoints.HasTag(SavePointData.Tag))
			{
				USavePointEntryData* NewEntryData = NewObject<USavePointEntryData>(this->GetWorld());
				NewEntryData->Tag = SavePointData.Tag;
				NewEntryData->DisplayName = SavePointData.DisplayName;
				Result.Add(NewEntryData);
			}
		}
	}
	
	return Result;
}
