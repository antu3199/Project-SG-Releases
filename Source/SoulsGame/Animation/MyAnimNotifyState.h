// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"

#include "MyAnimNotifyState.generated.h"


/**
 * 
 */
UCLASS()
class SOULSGAME_API UMyAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	


public:

	UMyAnimNotifyState();

	UPROPERTY(EditAnywhere)
	bool bIsAutoGenerated = false;

	bool GetHasEnded() const { return bHasEnded; }
	
protected:

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;


	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration);

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);


	virtual bool ShouldDoNotify(USkeletalMeshComponent * MeshComp);


	float CachedTime = 0;

	bool HasTriggered = false;


	UPROPERTY(EditAnywhere)
	float MultiNotifyThresh = 0.01f;

	virtual ASCharacterBase* GetCharacter(USkeletalMeshComponent* MeshComp);
	
	virtual ASHumanoidCharacter* GetHumanoidCharacter(USkeletalMeshComponent* MeshComp);

	virtual ASHumanoidPlayerCharacter* GetHumanoidPlayerCharacter(USkeletalMeshComponent* MeshComp);

	bool IsPreviewingInEditor() const;

	UPROPERTY(EditAnywhere)
	bool DisableMe = false;

	// Assumes that there is only one component on the socket, which might not be the case.
	USceneComponent * GetComponentOnSocket(USkeletalMeshComponent* MeshComp, FString SocketName);

	FAnimNotifyEventReference AnimNotifyEventReference;
	bool bHasEnded = false;
	
};
