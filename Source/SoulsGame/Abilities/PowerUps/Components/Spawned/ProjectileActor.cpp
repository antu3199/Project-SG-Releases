// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileActor.h"

#include "AbilitySystemGlobals.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "SoulsGame/Projectiles/SProjectileMovementComponent.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(Root);

	UArrowComponent * ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(Base);
    ArrowComponent->SetHiddenInGame(true);


	ProjectileMovement = CreateDefaultSubobject<USProjectileMovementComponent>(TEXT("ProjectileMovement"));
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EmitterParticleSystem"));
	NiagaraComponent->SetupAttachment(Base);

}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	OnDestroyed.AddDynamic(this, &AProjectileActor::OnActorDestroyed);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]()
	{
		this->Destroy();
	});

	GetWorld()->GetTimerManager().SetTimer(DestroySelfHandle, TimerCallback, LifeTime, false, LifeTime);
}


// Note: Not used anymore for now, but might be useful for AOE effects...
/*
void AProjectileActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor == GetInstigator())
	{
		return;
	}

	if (OtherActor->GetOwner() == GetInstigator())
	{
		return;
	}


}

void AProjectileActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
}
*/

void AProjectileActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Cannot hit self (Don't use ShouldApplyEffects because this is block)

	// AActor* ParentActor = MyComp->GetOwner();
	// if (ParentActor == Other)
	// {
	// 	return;
	// }
	
	PlayParticleHitAtLocation(HitLocation);
	this->Destroy();
}

void AProjectileActor::DisableHomingAfterSeconds(float Seconds)
{
	if (FMath::IsNearlyEqual(Seconds, -1))
	{
		return;
	}
	
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]()
    {
		if (this && this->ProjectileMovement)
		{
			this->ProjectileMovement->bIsHomingProjectile = false;
			this->ProjectileMovement->HomingTargetComponent = nullptr;
			this->ProjectileMovement->HomingAccelerationMagnitude = 0;
		}

    });

	GetWorld()->GetTimerManager().SetTimer(DisableHomingHandle, TimerCallback, Seconds, false, Seconds);
}

TWeakPtr<FGameplayEffectsWrapperContainer> AProjectileActor::GetGameplayEffectsWrapperContainer()
{
	return Super::GetGameplayEffectsWrapperContainer();
}

void AProjectileActor::DoDestroyEffectInPlace()
{
	this->PlayParticleHitAtLocation(GetActorLocation());

	this->Destroy();
}



void AProjectileActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Applies the actual gameplay effect
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
/*
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	if (OtherActor == GetInstigator())
	{
		return;
	}
*/

	// Destroys VFX
	// TODO: Allow for effect index specification
	if (!CanApplyEffectToActor(0, OtherActor))
	{
		return;
	}
	
    this->PlayParticleHitAtLocation(SweepResult.Location);

	if (bDestroyOnHit)
	{
		this->Destroy();
	}
}


void AProjectileActor::OnActorDestroyed(AActor* DestroyedActor)
{
	GetWorld()->GetTimerManager().ClearTimer(DisableHomingHandle);
}

void AProjectileActor::PlayParticleHitAtLocation(const FVector Location)
{
	const FTransform SpawnTransform(Location);

	if (PlayCascadeSystem && CascadeCollisionTemplate)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CascadeCollisionTemplate, SpawnTransform, true, EPSCPoolMethod::None, true );

	if (PlayNiagaraParticleSystem && NiagaraCollisionTemplate)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraCollisionTemplate, SpawnTransform.GetLocation());

	if (DestroySelfHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroySelfHandle);
	}

	if (DisableHomingHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DisableHomingHandle);
	}
}



