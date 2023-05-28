#include "SavePointMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGameInstance.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/Level/SavePointActor.h"
#include "SoulsGame/SaveLoad/SGSaveLoadManager.h"

USavePointMenuWidget::USavePointMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USavePointMenuWidget::CustomInitialize()
{
}

void USavePointMenuWidget::DoQuickTravel()
{
}

void USavePointMenuWidget::DoSave()
{
}

void USavePointMenuWidget::DoLoad()
{
}

void USavePointMenuWidget::DoReturnToGame()
{
}

void USavePointMenuWidget::ExitToMainMenu()
{
}

void USavePointMenuWidget::Debug_UnlockAll()
{
	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		const UStaticSGData* StaticData = GameInstance->GetStaticData();
		for (const FSavePointData& SavePointData : StaticData->SavePointData)
		{
			GameInstance->GetSaveLoadManager()->GetCurrentSaveRecordMutable().UnlockedSavePoints.AddTag(SavePointData.Tag);
		}
	}
}

TArray<USavePointEntryData*> USavePointMenuWidget::GetSavePointEntryData() const
{
	TArray<USavePointEntryData*> Result;
	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
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
