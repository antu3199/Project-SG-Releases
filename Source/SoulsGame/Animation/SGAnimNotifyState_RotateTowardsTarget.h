#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_RotateTowardsTarget.generated.h"

UCLASS()
class USGAnimNotifyState_RotateTowardsTarget : public USGAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;


	UPROPERTY(EditAnywhere)
	bool bLockZAxis = false;
	
	virtual  void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
