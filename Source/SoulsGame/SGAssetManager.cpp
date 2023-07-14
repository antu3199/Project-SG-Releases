// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAssetManager.h"
#include "AbilitySystemGlobals.h"

USGAssetManager::USGAssetManager()
{
}

void USGAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

    UAbilitySystemGlobals::Get().InitGlobalData();
}

USGAssetManager& USGAssetManager::Get()
{
    USGAssetManager * AssetManager = Cast<USGAssetManager>(GEngine->AssetManager);

    if (AssetManager)
    {
        return *AssetManager;
    }
    else
    {
        UE_LOG(LogTemp, Fatal, TEXT("AssetManager is not UMyAssetManager!"));
        return *NewObject<USGAssetManager>();
    }
}

UObject* USGAssetManager::ForceLoad(const FPrimaryAssetId& PrimaryAssetId)
{
    const FPrimaryAssetData * NameData = GetNameData(PrimaryAssetId);
    const FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

     // This does a synchronous load and may hitch
    return ItemPath.TryLoad();
}



