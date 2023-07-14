// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_SendGameplayTagEvent.generated.h"

/**
 * 
 */


UCLASS(BlueprintType)
class SOULSGAME_API USGSetEffectLevel_GameplayEventDataObject : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	int32 GetEffectIndex() const { return EffectIndex; }

	UFUNCTION(BlueprintCallable)
	int32 GetEffectLevel() const { return Level; }
	
	int32 EffectIndex = 0;
	int32 Level = 0;
};

UCLASS()
class SOULSGAME_API USGAnimNotifyState_SendGameplayTagEvent : public USGAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameplayTag GameplayTag;
	
	UPROPERTY(EditAnywhere)
	FSGAbilityEventData EventPayload;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag OnEndGameplayTag;

	UPROPERTY(EditAnywhere)
	FSGAbilityEventData EventEndPayload;
	
	UPROPERTY(EditAnywhere, Category = "Optional Objects|SetEffectLevel")
	bool bSetLevel = false;

	UPROPERTY(EditAnywhere, Category = "Optional Objects|SetEffectLevel")
	int32 SetLevelEffectIndex = 0;
	
	UPROPERTY(EditAnywhere, Category = "Optional Objects|SetEffectLevel")
	int32 SetLevelEffectLevel = 0;

protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

	USGSetEffectLevel_GameplayEventDataObject* ConstructSetEffectLevel() const;
	
};
