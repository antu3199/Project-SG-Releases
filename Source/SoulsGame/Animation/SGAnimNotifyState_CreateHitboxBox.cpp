#include "SGAnimNotifyState_CreateHitboxBox.h"

#include "SoulsGame/SGUtils.h"


int32 USGAnimNotifyState_CreateHitboxBox::CreateHitbox(USkeletalMeshComponent* MeshComp, const FSGHitboxParams& Params)
{
	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(MeshComp->GetWorld()))
	{
		return HitboxSubsystem->CreateBox(Params, MeshComp, SocketName.ToString(), BoxExtents, AttachmentRule, TranslationOffset, RotationOffset);
	}
	
	return INDEX_NONE;
}

