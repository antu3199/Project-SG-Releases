#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_LockVelocityIfInAir.generated.h"

UCLASS()
class USGAnimNotifyState_LockVelocityIfInAir : public USGAnimNotifyState
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
