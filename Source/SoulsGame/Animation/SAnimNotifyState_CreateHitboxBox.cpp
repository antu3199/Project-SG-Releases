#include "SAnimNotifyState_CreateHitboxBox.h"

#include "SoulsGame/SHitboxManager.h"


int32 USAnimNotifyState_CreateHitboxBox::CreateHitbox(USkeletalMeshComponent* MeshComp, USHitboxManager* HitboxManager)
{
	check(HitboxManager);
	return HitboxManager->CreateBox(MeshComp, SocketName.ToString(), BoxExtents, AttachmentRule, TranslationOffset, RotationOffset);
}
