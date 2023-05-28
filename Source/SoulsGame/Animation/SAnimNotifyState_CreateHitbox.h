#pragma once
#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "SoulsGame/SGTeam.h"


#include "SAnimNotifyState_CreateHitbox.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class SOULSGAME_API USAnimNotifyState_CreateHitbox : public UMyAnimNotifyState
{
	GENERATED_BODY()

public:

	// The name of the ability sent in the event tag
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
	
	// The socket within our mesh component to attach to when we spawn the Niagara component
	UPROPERTY(EditAnywhere, Category = NiagaraSystem, meta = (ToolTip = "The socket or bone to attach the system to", AnimNotifyBoneName = "true"))
	FName SocketName;

	UPROPERTY(EditAnywhere)
	EAttachmentRule AttachmentRule;

	UPROPERTY(EditAnywhere)
	FVector TranslationOffset;

	UPROPERTY(EditAnywhere)
	FQuat RotationOffset;

	UPROPERTY(EditAnywhere)
	FSGCollisionParams CollisionParams;

	UPROPERTY(EditAnywhere)
	bool bOnlyActivateInPreview = false;

	UPROPERTY(EditAnywhere)
	float HitboxOverlapInterval = -1.0f;
	
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, class USHitboxManager* HitboxManager);

	int32 HitboxId = -1;
};
