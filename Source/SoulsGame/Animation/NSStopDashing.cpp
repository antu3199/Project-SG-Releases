// Fill out your copyright notice in the Description page of Project Settings.


#include "NSStopDashing.h"

void UNSStopDashing::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
}

void UNSStopDashing::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::DoNotifyEnd(MeshComp, Animation);

    if (MeshComp == nullptr)
    {
        return;
    }
    
    ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
    if (Character == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR: No character!"));
        return;
    }
    Character->StopRolling();
}
