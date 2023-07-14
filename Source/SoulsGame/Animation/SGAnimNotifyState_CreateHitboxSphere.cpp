#include "SGAnimNotifyState_CreateHitboxSphere.h"


/*
int32 USAnimNotifyState_CreateHitboxSphere::CreateHitbox(USkeletalMeshComponent* MeshComp, USHitboxManager* HitboxManager)
{
	check(HitboxManager);
	return HitboxManager->CreateSphere(MeshComp, SocketName.ToString(), Radius, AttachmentRule, TranslationOffset, RotationOffset);
}
*/

int32 USGAnimNotifyState_CreateHitboxSphere::CreateHitbox(USkeletalMeshComponent* MeshComp,
	const FSGHitboxParams& Params)
{
	// TODO:
	return INDEX_NONE;
}
