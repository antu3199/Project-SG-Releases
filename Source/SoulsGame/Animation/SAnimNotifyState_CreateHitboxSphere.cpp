#include "SAnimNotifyState_CreateHitboxSphere.h"

#include "SoulsGame/SHitboxManager.h"


int32 USAnimNotifyState_CreateHitboxSphere::CreateHitbox(USkeletalMeshComponent* MeshComp, USHitboxManager* HitboxManager)
{
	check(HitboxManager);
	return HitboxManager->CreateSphere(MeshComp, SocketName.ToString(), Radius, AttachmentRule, TranslationOffset, RotationOffset);
}
