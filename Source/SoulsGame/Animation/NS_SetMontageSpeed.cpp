// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_SetMontageSpeed.h"

#include "SoulsGame/SUtils.h"

void UNS_SetMontageSpeed::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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

	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}

	
	BasePlayRate = RootMotion->GetPlayRate();

	bool bIsTimestopped = FSUtils::bIsTimestopped;

	float MontageSpeed = SetPlayRate;

	if ((bUseTimestoppedPlayRate && bIsTimestopped) || bDebug_UseTimestoppedRate)
	{
		MontageSpeed = TimestoppedPlayRate;
	}

	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(GetCharacter(MeshComp)))
	{
		CharacterBase->SetCurrentMontageSpeed(MontageSpeed);
		MontageSpeed = CharacterBase->GetFinalMontageSpeed();
	}
	
	RootMotion->SetPlayRate(MontageSpeed);
}

void UNS_SetMontageSpeed::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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

	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}


	float Speed = BasePlayRate;
	
	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(GetCharacter(MeshComp)))
	{
		CharacterBase->SetCurrentMontageSpeed(Speed);
		Speed = CharacterBase->GetFinalMontageSpeed();
	}
	
	RootMotion->SetPlayRate(BasePlayRate);
}
