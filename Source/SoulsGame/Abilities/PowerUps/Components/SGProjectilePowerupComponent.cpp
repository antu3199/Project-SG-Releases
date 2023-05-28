#include "SGProjectilePowerupComponent.h"

#include "SGPowerupStats.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SUtils.h"
#include "Spawned/ProjectileActor.h"


USGProjectilePowerupComponent::USGProjectilePowerupComponent() : Super()
{
}

void USGProjectilePowerupComponent::OnInitializeAbilityContext(USGPowerupAbility* Ability, AActor* InAbilitySystemOwner,
	AActor* InComponentOwner)
{
	Super::OnInitializeAbilityContext(Ability, InAbilitySystemOwner, InComponentOwner);

	ProjectileOrigin = FSUtils::GetComponentByName<USceneComponent>(InComponentOwner, ProjectileSceneComponentName.ToString());

	if (ProjectileOrigin == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[USGProjectilePowerupComponent::OnInitializeAbilityContext] Missing SGProjectilePowerup_SceneComponent0"));
	}
}

void USGProjectilePowerupComponent::SetHomingTarget(USceneComponent* InTarget)
{
	this->HomingTarget = InTarget;
}

void USGProjectilePowerupComponent::ResetPowerupTimer()
{
	LastShotTime = 0;
}

void USGProjectilePowerupComponent::DoPowerup()
{
	Super::DoPowerup();

	SingleShot(ShootDirection);
}

void USGProjectilePowerupComponent::SingleShot(const FVector& OverrideDirection)
{
	if (ProjectileOrigin == nullptr)
	{
		return;
	}
	
	AActor * Owner = GetOwner();

	FTransform ComponentTransform = ProjectileOrigin->GetComponentTransform();
	ComponentTransform.SetScale3D(FVector(1,1,1));
	ComponentTransform.AddToTranslation(TransformOffset);
	
	//Spawn Actor
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Default
	SpawnParameters.bNoFail = true;
	SpawnParameters.Instigator = Cast<APawn>(Owner);
	// UE_LOG(LogTemp, Warning, TEXT("Transform %s"), *ComponentTransform.ToString());

	AProjectileActor * SpawnedObject = GetWorld()->SpawnActor<AProjectileActor>(this->ProjectileActorTemplate, ComponentTransform, SpawnParameters);
	if (SpawnedObject)
	{
		FVector Velocity = SpawnedObject->GetVelocity();
		FVector Velocity2 = SpawnedObject->GetProjectileMovement()->Velocity;

		UProjectileMovementComponent * MovementComponent = SpawnedObject->GetProjectileMovement();
		if (MovementComponent != nullptr && OverrideDirection != FVector::ZeroVector)
		{
			MovementComponent->Velocity = OverrideDirection * MovementComponent->InitialSpeed;
		}

		UProjectileMovementComponent * ProjectileComponent = SpawnedObject->GetProjectileMovement();
		if (IsHomingProjectile == true && HomingTarget != nullptr)
		{
			ProjectileComponent->bIsHomingProjectile = true;
			ProjectileComponent->HomingTargetComponent = HomingTarget;
			ProjectileComponent->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
			SpawnedObject->DisableHomingAfterSeconds(DisableHomingAfterSeconds);
		}
		else
		{
			ProjectileComponent->bIsHomingProjectile = false;
		}

		SpawnedObject->SetGameplayEffectDataContainer(GameplayEffectsContainer);

		
		
		OnShotSpawn(SpawnedObject);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to spawn object %s"), *GetName());
	}
}

void USGProjectilePowerupComponent::DoBarrage(float TimeBetweenShots, float NumShots)
{
	int * CurNumShots = new int(0);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]()
	{
		bool bForceEndBarrage = false;
		if (CanDoPowerup())
		{
			RequestPowerup();
		}
		else
		{
			bForceEndBarrage = true;
		}
		
		int CurrentNumShots = *CurNumShots;
		*CurNumShots = CurrentNumShots + 1;

		if (*CurNumShots >= NumShots || bForceEndBarrage)
		{
			delete CurNumShots;
			GetWorld()->GetTimerManager().ClearTimer(BarrageHandle);
			OnEndBarrage();
		}
	});

	GetWorld()->GetTimerManager().SetTimer(BarrageHandle, TimerCallback, TimeBetweenShots, true);

}

void USGProjectilePowerupComponent::OnEndBarrage()
{
}

void USGProjectilePowerupComponent::OnShotSpawn(AProjectileActor* SpawnedObject)
{
}


FVector USGProjectilePowerupComponent::GetShooterToTargetDirection(const FVector& InTarget) const
{
	if (InTarget == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Error, TEXT("INVALID TARGET VECTOR!"));
	}
	FVector Direction = FVector::ZeroVector;

	Direction = TargetVector - ProjectileOrigin->GetComponentLocation();
	Direction.Normalize();

	return Direction;
}

FVector USGProjectilePowerupComponent::GetShooterToTargetDirection(USceneComponent* InTarget) const
{
	FVector Direction = FVector::ZeroVector;
	Direction = InTarget->GetComponentLocation() - ProjectileOrigin->GetComponentLocation();
	Direction.Normalize();
	return Direction;
}

USceneComponent* USGProjectilePowerupComponent::GetProjectileOrigin() const
{
	return ProjectileOrigin;
}


FVector USGProjectilePowerupComponent::GetShooterToTargetDirection() const
{
	if (Target != nullptr)
	{
		return GetShooterToTargetDirection(Target);
	}
	else
	{
		return GetShooterToTargetDirection(TargetVector);
	}
}
