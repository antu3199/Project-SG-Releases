#pragma once
#include "CoreMinimal.h"

#include "HitboxGameplayEventDataObject.generated.h"

UCLASS()
class SOULSGAME_API USGCreateHitbox_GameplayEventDataObject : public UObject
{
	GENERATED_BODY()
public:
	int32 HitboxId = -1;
	float OverlapInterval = -1.0f;
};
