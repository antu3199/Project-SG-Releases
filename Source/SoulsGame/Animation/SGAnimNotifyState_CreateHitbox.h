#pragma once
#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SoulsGame/SGTeam.h"
#include "SoulsGame/Subsystems/SGHitboxSubsystem.h"


#include "SGAnimNotifyState_CreateHitbox.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class SOULSGAME_API USGAnimNotifyState_CreateHitbox : public USGAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FSGHitboxParams HitboxParams;
	
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
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere)
	FSGCollisionParams CollisionParams;

	UPROPERTY(EditAnywhere)
	bool bOnlyActivateInPreview = false;

	UPROPERTY(EditAnywhere)
	float HitboxOverlapInterval = -1.0f;
	
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual int32 CreateHitbox(USkeletalMeshComponent* MeshComp, const FSGHitboxParams& Params);

	int32 HitboxId = -1;
};
