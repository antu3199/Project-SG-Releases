// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileShooterCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Abilities/SShootAbility.h"

ASProjectileShooterCharacter::ASProjectileShooterCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    OriginSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileOriginComponent"));
    OriginSceneComponent->SetupAttachment(GetMesh());
}


void ASProjectileShooterCharacter::BeginDestroy()
{
    Super::BeginDestroy();

}

void ASProjectileShooterCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    //const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true );
    const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false );
    OriginSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(ProjectileSpawnerParentSocket));
}


FVector ASProjectileShooterCharacter::GetDefaultTarget() const
{
    /*
    bool bIsEnemyCharacter = true;
    if (bIsEnemyCharacter)
    {
        APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            return PlayerPawn->GetActorLocation();
        }
    }

    // See Confecitonite
    */
    USceneComponent* HomingTarget = GetDefaultHomingTarget();
    if (HomingTarget == nullptr)
    {
        return FVector::ZeroVector;
    }
    
    return HomingTarget->GetComponentLocation();
}

USceneComponent* ASProjectileShooterCharacter::GetDefaultHomingTarget() const
{
    return FocusTarget;
}

UAbilitySystemComponent* ASProjectileShooterCharacter::GetAbilitySystemComponent() const
{
    return this->AbilitySystemComponent;
}

void ASProjectileShooterCharacter::BeginPlay()
{
    Super::BeginPlay();
    // DebugGameplayAbility = USShootAbility::StaticClass();
    // CachedAbilitySpecWrapper = &AbilitySystemComponent->GrantAbility(DebugGameplayAbility);
    
    GetComponents<UProjectileShooterComponent>(ProjectileSpawners);
    InitializeSShooter(OriginSceneComponent);
    
    bool bDebugThing = false;
    if (bDebugThing)
    {

        UProjectileShooterComponent * ShotComponent = GetProjectileSpawner("PShooter_LinearProjectileShooter");
        ShotComponent->bPlayLooping = true;
        ShotComponent->StartShooting();
    }

}

void ASProjectileShooterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}



