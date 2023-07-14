#pragma once
#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_CreateHitbox.h"
#include "SoulsGame/SGCollisionProfile.h"

#include "SGAnimNotifyState_CreateHitboxCapsule.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSGAME_API USGAnimNotifyState_CreateHitboxCapsule : public USGAnimNotifyState_CreateHitbox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 1.0f;

	UPROPERTY(EditAnywhere)
	float CapsuleRadius = 1.0f;
	
	
protected:
	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, const FSGHitboxParams& Params) override;
};
