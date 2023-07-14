// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SGAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
    USGAssetManager();

    virtual void StartInitialLoading() override;

    static USGAssetManager & Get();

    UObject * ForceLoad(const FPrimaryAssetId& PrimaryAssetId);

    template <class T>
    T * ForceLoad(const FPrimaryAssetId & PrimaryAssetId);
};

template <class T>
T* USGAssetManager::ForceLoad(const FPrimaryAssetId& PrimaryAssetId)
{
    T * LoadedItem = Cast<T>(this->ForceLoad(PrimaryAssetId));
    if (LoadedItem == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast asset: %s"), *PrimaryAssetId.ToString() );
    }

    return LoadedItem;
}
