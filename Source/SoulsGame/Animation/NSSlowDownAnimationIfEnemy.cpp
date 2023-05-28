// Fill out your copyright notice in the Description page of Project Settings.


#include "NSSlowDownAnimationIfEnemy.h"

void UNSSlowDownAnimationIfEnemy::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASCharacterBase * Character = GetCharacter(MeshComp);

	if ((!Character || Character->IsPlayerControlled()) && !DebugAnimation)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NSSlowDownAnimationIfEnemy: RootMotion null"));
		return;
	}

	BasePlayRate = RootMotion->GetPlayRate();

	const float SetPlayRate = FMath::RandRange(SetAnimationRateMin, SetAnimationRateMax);

	RootMotion->SetPlayRate(SetPlayRate);
	
}

void UNSSlowDownAnimationIfEnemy::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASCharacterBase * Character = GetCharacter(MeshComp);

	if ((!Character || Character->IsPlayerControlled()) && !DebugAnimation)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NSSlowDownAnimationIfEnemy: RootMotion null"));
		return;
	}

	RootMotion->SetPlayRate(BasePlayRate);
}

void UNSSlowDownAnimationIfEnemy::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NSSlowDownAnimationIfEnemy: RootMotion null"));
		return;
	}

	//const float PlayRate = RootMotion->GetPlayRate();
	//const float NewPlayRate = FMath::Lerp(PlayRate, BasePlayRate, 0.05f);
	//RootMotion->SetPlayRate(NewPlayRate);
}
