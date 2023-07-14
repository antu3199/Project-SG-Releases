#pragma once
#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_CreateHitbox.h"
#include "SoulsGame/SGCollisionProfile.h"


#include "SGAnimNotifyState_CreateHitboxBox.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSGAME_API USGAnimNotifyState_CreateHitboxBox : public USGAnimNotifyState_CreateHitbox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FVector BoxExtents = FVector(1,1,1);
	
protected:
	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, const FSGHitboxParams& Params) override;
};
