#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "NSSG_RotateTowardsTarget.generated.h"

UCLASS()
class UNSSG_RotateTowardsTarget : public UMyAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;


	UPROPERTY(EditAnywhere)
	bool bLockZAxis = false;
	
	virtual  void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
