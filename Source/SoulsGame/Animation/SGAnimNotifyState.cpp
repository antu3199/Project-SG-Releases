// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState.h"


#include "GameplayTagsManager.h"
#include "SoulsGame/Character/SGCharacterBase.h"

USGAnimNotifyState::USGAnimNotifyState()
{
	this->HasTriggered = false;
	this->CachedTime = 0;
}

void USGAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	bHasEnded = false;
	AnimNotifyEventReference = FAnimNotifyEventReference(EventReference);
	
	if (this->ShouldDoNotify(MeshComp))
	{
		this->DoNotifyBegin(MeshComp, Animation, TotalDuration);
	}
	else
	{
		if (!DisableMe)
		{
			UE_LOG(LogTemp, Warning, TEXT("Notify ignored!"));
		}
	}
}

void USGAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (DisableMe)
	{
		return;
	}

	if (bHasEnded)
	{
		return;
	}
	
	ASGCharacterBase * Character = GetCharacter(MeshComp);
	if (!Character)
	{
		this->DoNotifyEnd(MeshComp, Animation);
		return;
	}
	
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		HasTriggered = false;
		this->DoNotifyEnd(MeshComp, Animation);
		return;
	}

	
	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		HasTriggered = false;
		this->DoNotifyEnd(MeshComp, Animation);
		return;
	}

	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (CurrentActiveMontage)
	{
		bool HackyMetaData = false;
		for (auto MetaData : CurrentActiveMontage->GetMetaData())
		{
			if (MetaData == nullptr)
			{
				HackyMetaData = true;
			}
		}

		if (HackyMetaData)
		{
			return;
		}
	}
	
	if (RootMotion->bPlaying == false)
	{
		this->CachedTime = RootMotion->GetPosition();
		this->HasTriggered = true;
	}
	else
	{
		this->DoNotifyEnd(MeshComp, Animation);
		bHasEnded = true;
	}
}


void USGAnimNotifyState::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration)
{

	// Override me!

}

void USGAnimNotifyState::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// OVerride me!
}

bool USGAnimNotifyState::ShouldDoNotify(USkeletalMeshComponent* MeshComp)
{
	if (DisableMe)
	{
		return false;
	}
	
	ASGCharacterBase * Character = GetCharacter(MeshComp);
	if (!Character)
	{
		return true;
	}
	
	if (this->CachedTime == 0)
	{
		return true;
	}
	
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return true;
	}

	
	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		return true;
	}


	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();

	if (CurrentActiveMontage)
	{
		bool HackyMetaData = false;
		for (auto MetaData : CurrentActiveMontage->GetMetaData())
		{
			if (MetaData == nullptr)
			{
				HackyMetaData = true;
			}
		}

		if (HackyMetaData)
		{
			return false;
		}
	}
	
	
	const float Abs = FMath::Abs(RootMotion->GetPosition() - this->CachedTime);

	if (Abs > this->MultiNotifyThresh)
	{
		this->HasTriggered = false;
	}

	return !this->HasTriggered;

}


ASGCharacterBase * USGAnimNotifyState::GetCharacter(USkeletalMeshComponent* MeshComp)
{
	return Cast<ASGCharacterBase>(MeshComp->GetOwner());
}

ASGHumanoidCharacter* USGAnimNotifyState::GetHumanoidCharacter(USkeletalMeshComponent* MeshComp)
{
	return Cast<ASGHumanoidCharacter>(MeshComp->GetOwner());
}

ASGHumanoidPlayerCharacter* USGAnimNotifyState::GetHumanoidPlayerCharacter(USkeletalMeshComponent* MeshComp)
{
	return Cast<ASGHumanoidPlayerCharacter>(MeshComp->GetOwner());
}

bool USGAnimNotifyState::IsPreviewingInEditor() const
{
#if WITH_EDITOR
	return !GIsPlayInEditorWorld;
#endif
	return false;
}

USceneComponent* USGAnimNotifyState::GetComponentOnSocket(USkeletalMeshComponent* MeshComp, FString SocketName)
{

	for (auto SceneComp : MeshComp->GetAttachChildren())
	{
		if (SceneComp->GetAttachSocketName().ToString() == SocketName)
		{
		    return SceneComp;
		}
	}

	return nullptr;
}

