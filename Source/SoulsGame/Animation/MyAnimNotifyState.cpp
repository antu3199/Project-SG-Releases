// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimNotifyState.h"


#include "GameplayTagsManager.h"
#include "SoulsGame/Character/SCharacterBase.h"

UMyAnimNotifyState::UMyAnimNotifyState()
{
	this->HasTriggered = false;
	this->CachedTime = 0;
}

void UMyAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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

void UMyAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
	
	ASCharacterBase * Character = GetCharacter(MeshComp);
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


void UMyAnimNotifyState::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration)
{

	// Override me!

}

void UMyAnimNotifyState::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// OVerride me!
}

bool UMyAnimNotifyState::ShouldDoNotify(USkeletalMeshComponent* MeshComp)
{
	if (DisableMe)
	{
		return false;
	}
	
	ASCharacterBase * Character = GetCharacter(MeshComp);
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


ASCharacterBase * UMyAnimNotifyState::GetCharacter(USkeletalMeshComponent* MeshComp)
{
	return Cast<ASCharacterBase>(MeshComp->GetOwner());
}

ASHumanoidCharacter* UMyAnimNotifyState::GetHumanoidCharacter(USkeletalMeshComponent* MeshComp)
{
	return Cast<ASHumanoidCharacter>(MeshComp->GetOwner());
}

ASHumanoidPlayerCharacter* UMyAnimNotifyState::GetHumanoidPlayerCharacter(USkeletalMeshComponent* MeshComp)
{
	return Cast<ASHumanoidPlayerCharacter>(MeshComp->GetOwner());
}

bool UMyAnimNotifyState::IsPreviewingInEditor() const
{
#if WITH_EDITOR
	return !GIsPlayInEditorWorld;
#endif
	return false;
}

USceneComponent* UMyAnimNotifyState::GetComponentOnSocket(USkeletalMeshComponent* MeshComp, FString SocketName)
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

