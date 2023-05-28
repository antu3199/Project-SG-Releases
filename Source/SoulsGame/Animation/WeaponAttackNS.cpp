// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAttackNS.h"



#include "GameplayTagsManager.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/DataAssets/WeaponAsset.h"
#include "SoulsGame/Weapon/WeaponActor.h"

void UWeaponAttackNS::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
    
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
    
    AWeaponActor * WeaponActor = Character->WeaponActor;
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

void UWeaponAttackNS::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::DoNotifyEnd(MeshComp, Animation);

    if (MeshComp == nullptr)
    {
        return;
    }

    ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
    if (!Character)
    {
        return;
    }
    
    AWeaponActor * WeaponActor = Character->WeaponActor;
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
