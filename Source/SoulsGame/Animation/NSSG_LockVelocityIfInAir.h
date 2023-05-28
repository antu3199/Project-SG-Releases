#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "NSSG_LockVelocityIfInAir.generated.h"

UCLASS()
class UNSSG_LockVelocityIfInAir : public UMyAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	UPROPERTY(EditAnywhere)
	bool bModifyRootMotionTranslationScale = false;

	bool bAppliedVelocity = false;
	float CachedTranslationScale = 1.0f;
};
