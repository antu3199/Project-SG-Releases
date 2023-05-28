#pragma once
#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "SAnimNotifyState_CreateHitbox.h"
#include "SoulsGame/SGCollisionProfile.h"

#include "SAnimNotifyState_CreateHitboxCapsule.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSGAME_API USAnimNotifyState_CreateHitboxCapsule : public USAnimNotifyState_CreateHitbox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 1.0f;

	UPROPERTY(EditAnywhere)
	float CapsuleRadius = 1.0f;
	
	
protected:
	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, class USHitboxManager* HitboxManager) override;
};
