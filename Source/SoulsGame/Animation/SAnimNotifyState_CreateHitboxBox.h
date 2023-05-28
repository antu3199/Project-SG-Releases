#pragma once
#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "SAnimNotifyState_CreateHitbox.h"
#include "SoulsGame/SGCollisionProfile.h"


#include "SAnimNotifyState_CreateHitboxBox.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSGAME_API USAnimNotifyState_CreateHitboxBox : public USAnimNotifyState_CreateHitbox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FVector BoxExtents = FVector(1,1,1);
	
protected:
	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, class USHitboxManager* HitboxManager) override;
};
