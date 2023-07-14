// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_SetMontageSpeed.h"

#include "SoulsGame/SGUtils.h"

void USGAnimNotifyState_SetMontageSpeed::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                       float TotalDuration)
{

	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
	
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_SetMontageSpeed: RootMotion null"));
		return;
	}

	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}

	
	BasePlayRate = RootMotion->GetPlayRate();

	bool bIsTimestopped = FSGUtils::bIsTimestopped;

	float MontageSpeed = SetPlayRate;

	if ((bUseTimestoppedPlayRate && bIsTimestopped) || bDebug_UseTimestoppedRate)
	{
		MontageSpeed = TimestoppedPlayRate;
	}

	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		CharacterBase->SetCurrentMontageSpeed(MontageSpeed);
		MontageSpeed = CharacterBase->GetFinalMontageSpeed();
	}
	
	RootMotion->SetPlayRate(MontageSpeed);
}

void USGAnimNotifyState_SetMontageSpeed::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_SetMontageSpeed: RootMotion null"));
		return;
	}

	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}


	float Speed = BasePlayRate;
	
	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		CharacterBase->SetCurrentMontageSpeed(Speed);
		Speed = CharacterBase->GetFinalMontageSpeed();
	}
	
	RootMotion->SetPlayRate(BasePlayRate);
}
