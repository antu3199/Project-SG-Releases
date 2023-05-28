// Fill out your copyright notice in the Description page of Project Settings.


#include "NSChangeActorRoot.h"


void UNSChangeActorRoot::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASHumanoidPlayerCharacter * CharacterBase = GetHumanoidPlayerCharacter(MeshComp);
	
	if (!CharacterBase)
		return;

	if (SocketName != "")
	{
		if (SnapToTarget)
		{
			const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false );
			//CharacterBase->ChangeConfectioniteParent(AttachmentRules, SocketName);
		}
		else
		{
			const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true );
			//CharacterBase->ChangeConfectioniteParent(AttachmentRules, SocketName);
		}
	}
	else
	{
		const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
		//CharacterBase->DetachConfectioniteCharacterTransform();
	}

	
}

void UNSChangeActorRoot::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}
