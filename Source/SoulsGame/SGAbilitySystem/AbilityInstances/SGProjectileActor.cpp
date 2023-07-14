// Fill out your copyright notice in the Description page of Project Settings.


#include "SGProjectileActor.h"

#include "AbilitySystemGlobals.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Projectiles/SGProjectileMovementComponent.h"
#include "SoulsGame/Subsystems/SGHitboxSubsystem.h"

// Sets default values
ASGProjectileActor::ASGProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(Root);

	UArrowComponent * ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(Base);
    ArrowComponent->SetHiddenInGame(true);


	ProjectileMovement = CreateDefaultSubobject<USGProjectileMovementComponent>(TEXT("ProjectileMovement"));
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EmitterParticleSystem"));
	NiagaraComponent->SetupAttachment(Base);

}

void ASGProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
	{
		FSGHitboxParams HitboxParams;
		HitboxParams.CollisionParams.ProfileType = ESGCollisionProfileType::Profile_OverlapAll;
		HitboxParams.CollisionParams.OverlapBehaviour = ESGTeamOverlapBehaviour::OnlyEnemyPawns;
		HitboxParams.TickInterval = 1.0f;
		HitboxParams.EffectContext = EffectContext;
		HitboxParams.bStartEnabled = false;

		HitboxId = HitboxSubsystem->RegisterExistingComponent(HitboxParams, Base);
		HitboxSubsystem->OnHitboxTriggered.AddDynamic(this, &ASGProjectileActor::OnHitboxTriggered);
	}
	

	OnDestroyed.AddDynamic(this, &ASGProjectileActor::OnActorDestroyed);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]()
	{
		this->DoDestroyEffectInPlace();
	});

	GetWorld()->GetTimerManager().SetTimer(DestroySelfHandle, TimerCallback, LifeTime, false, LifeTime);
}

void ASGProjectileActor::BeginDestroy()
{
	if (HitboxId != INDEX_NONE)
	{
		if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
		{
			HitboxSubsystem->RemoveHitbox(HitboxId);
			HitboxSubsystem->OnHitboxTriggered.RemoveAll(this);
		}
	}

	HitboxId = INDEX_NONE;
	
	Super::BeginDestroy();
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

void ASGProjectileActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Cannot hit self (Don't use ShouldApplyEffects because this is block)

	// AActor* ParentActor = MyComp->GetOwner();
	// if (ParentActor == Other)
	// {
	// 	return;
	// }

	DoDestroyEffectInPlace();

}

void ASGProjectileActor::DisableHomingAfterSeconds(float Seconds)
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

void ASGProjectileActor::DoDestroyEffectInPlace()
{
	this->PlayParticleHitAtLocation(GetActorLocation());

	this->Destroy();
}


void ASGProjectileActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	// Applies the actual gameplay effect
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);


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
	*/
}


void ASGProjectileActor::SetEffectContext(FSGEffectInstigatorContext InEffectContext)
{
	Super::SetEffectContext(InEffectContext);

	if (HitboxId != INDEX_NONE)
	{
		if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
		{
			if (USGHitboxObject* HitboxObject = HitboxSubsystem->GetHitbox(HitboxId))
			{
				HitboxObject->SetEffectContext(InEffectContext);
				HitboxObject->SetEnabled(true);
			}
		}
	}
}

void ASGProjectileActor::OnActorDestroyed(AActor* DestroyedActor)
{
	GetWorld()->GetTimerManager().ClearTimer(DisableHomingHandle);
}

void ASGProjectileActor::PlayParticleHitAtLocation(const FVector Location)
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

void ASGProjectileActor::OnHitboxTriggered(USGHitboxObject* HitboxObject,
	const FSGHitboxOverlapContext& OverlapContext)
{
	if (OverlapContext.bAppliedEffect)
	{
		PlayParticleHitAtLocation(GetActorLocation());

		if (bDestroyOnHit)
		{
			this->Destroy();
		}
	}
}

