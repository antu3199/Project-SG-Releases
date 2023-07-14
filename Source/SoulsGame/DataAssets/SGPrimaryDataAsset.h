// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SGPrimaryDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    USGPrimaryDataAsset();

    // Need to create type in Project Settings/Asset Manager to be visible in editor
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
    FPrimaryAssetType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
    FText ItemName;

    // Need to override to create a unique name and be visible in editor.
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
