#include "SAnimNotifyState_CreateHitboxCapsule.h"

#include "SoulsGame/SHitboxManager.h"


int32 USAnimNotifyState_CreateHitboxCapsule::CreateHitbox(USkeletalMeshComponent* MeshComp, USHitboxManager* HitboxManager)
{
	check(HitboxManager);
	return HitboxManager->CreateCapsule(MeshComp, SocketName.ToString(), CapsuleHalfHeight, CapsuleRadius, AttachmentRule, TranslationOffset, RotationOffset);
}
