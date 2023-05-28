
#include "SGStaticDataEditorSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/Level/SavePointActor.h"
#include "UObject/ObjectSaveContext.h"

static FAutoConsoleCommand CCmdResaveCheckpointData = FAutoConsoleCommand(
	TEXT("SGStaticData.RegenerateAll"),
	TEXT("Regenerates dynamic runtime data"),
	FConsoleCommandDelegate::CreateStatic(&USGStaticDataEditorSubsystem::RegenerateAll));

void USGStaticDataEditorSubsystem::RegenerateAll()
{
	if ( USGStaticDataEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem< USGStaticDataEditorSubsystem>())
	{
		for (TObjectIterator<AActor> Itr; Itr; ++Itr)
		{
			if (*Itr != nullptr && IsValid(*Itr))
			{
				Subsystem->OnPresave(*Itr);
			}
		}
	}
}

void USGStaticDataEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	m_OnObjectSaveHandle = FCoreUObjectDelegates::OnObjectPreSave.AddUObject(this, &USGStaticDataEditorSubsystem::OnObjectPreSave);

	if (GEngine)
	{
		m_OnObjectDeletedHandle = GEngine->OnLevelActorDeleted().AddUObject(this, &USGStaticDataEditorSubsystem::OnObjectsDeleted);
	}
}

void USGStaticDataEditorSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FCoreUObjectDelegates::OnObjectPreSave.Remove(m_OnObjectSaveHandle);
}

void USGStaticDataEditorSubsystem::OnObjectPreSave(UObject* InAsset, FObjectPreSaveContext InPreSaveContext)
{
	OnPresave(InAsset);	
}

void USGStaticDataEditorSubsystem::OnObjectsDeleted(AActor* Actor)
{
}

void USGStaticDataEditorSubsystem::UpdateActor(const ASavePointActor& SavePoint)
{
}

void USGStaticDataEditorSubsystem::OnPresave(UObject* Asset)
{
	if (ASavePointActor* SavePoint = Cast<ASavePointActor>(Asset))
	{
		SavePoint->GetLevel()->GetPathName();
		UpdateSavepointTransform(SavePoint->SavePointTag, SavePoint->GetSavepointTransformData());
	}
}


void USGStaticDataEditorSubsystem::UpdateSavepointTransform(const FGameplayTag& Tag, const FSavePointTransformData& Data)
{
	if (UStaticSGData* StaticData = GetStaticDataFromPath())
	{
		for (FSavePointData& SavepointData : StaticData->SavePointData)
		{
			if (SavepointData.Tag == Tag)
			{
				if (!SavepointData.TransformData.Equals(Data))
				{
					SavepointData.TransformData = Data;

					StaticData->MarkPackageDirty();
				}

				break;
			}
		}
	}
}

UStaticSGData* USGStaticDataEditorSubsystem::GetStaticDataFromPath()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FName(TEXT("/Game/DataAssets/DA_StaticSGData.DA_StaticSGData")));
	if (AssetData.IsValid())
	{
		if (UStaticSGData* SGData = Cast<UStaticSGData>(AssetData.GetAsset()))
		{
			return SGData;
		}
	}

	return nullptr;
}
