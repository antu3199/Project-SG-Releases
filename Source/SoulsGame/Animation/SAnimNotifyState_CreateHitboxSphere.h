#pragma once
#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "SAnimNotifyState_CreateHitbox.h"
#include "SoulsGame/SGCollisionProfile.h"

#include "SAnimNotifyState_CreateHitboxSphere.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSGAME_API USAnimNotifyState_CreateHitboxSphere : public USAnimNotifyState_CreateHitbox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Radius = 1.0f;
	
protected:
	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, class USHitboxManager* HitboxManager) override;
};
