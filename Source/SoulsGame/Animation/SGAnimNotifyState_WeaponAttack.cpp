// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_WeaponAttack.h"



#include "GameplayTagsManager.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/DataAssets/SGWeaponAsset.h"
#include "SoulsGame/Weapon/SGWeaponActor.h"

void USGAnimNotifyState_WeaponAttack::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
    
    if (MeshComp == nullptr)
    {
        return;
    }
    
    ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
    if (Character == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR: No character!"));
        return;
    }
    
    ASGWeaponActor * WeaponActor = Character->WeaponActor;
    if (!WeaponActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR: No weapon actor!"));
        return;
    }

    if (!bVisualsOnly)
    {
        WeaponActor->BeginWeaponAttack();
    }
    else
    {
        WeaponActor->BroadcastWeaponBeginVisuals();
    }
}

void USGAnimNotifyState_WeaponAttack::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::DoNotifyEnd(MeshComp, Animation);

    if (MeshComp == nullptr)
    {
        return;
    }

    ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
    if (!Character)
    {
        return;
    }
    
    ASGWeaponActor * WeaponActor = Character->WeaponActor;
    if (!WeaponActor)
    {
        return;
    }

    if (!bVisualsOnly)
    {
        WeaponActor->EndWeaponAttack();
    }
    else
    {
        WeaponActor->BroadcastWeaponEndVisuals();
    }
    
}
