// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileShooterComponent.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Abilities/SShootAbility.h"
#include "SoulsGame/Abilities/PowerUps/Components/Spawned/ProjectileActor.h"

// Sets default values for this component's properties
UProjectileShooterComponent::UProjectileShooterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UProjectileShooterComponent::ActivateDebugEffects(UMyGameplayAbility* InAbility)
{
	if (InAbility == nullptr)
	{
	}
	else
	{
		for (auto & Effect : DebugGameplayEffects)
		{
			InAbility->GameplayEffectsContainer->AddEffect(InAbility, Effect.GetDefaultObject());
		}
		SetGameplayEffectsUsingContainer(InAbility->GameplayEffectsContainer);
	}
	

}

// Called when the game starts
void UProjectileShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UProjectileShooterComponent::SingleShot(const FVector& OverrideDirection)
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

void UProjectileShooterComponent::SetNextShotAsLast()
{
	bNextShotAsLast = true;

	if (LoopHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoopHandle);
		LoopHandle.Invalidate();
	}
}

void UProjectileShooterComponent::DoShot()
{
	if (PreShot())
	{
		SingleShot(ShootDirection);
	}
}

bool UProjectileShooterComponent::PreShot()
{
	OnPreShot.Broadcast();
	return true;
}

void UProjectileShooterComponent::OnEndBarrage()
{
}

void UProjectileShooterComponent::DoBarrage(float TimeBetweenShots, float NumShots)
{
	int * CurNumShots = new int(0);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]()
	{
		bool bForceEndBarrage = false;
		if (PreShot())
		{
            SingleShot(ShootDirection);
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

void UProjectileShooterComponent::Helper_SetProjectileToDirection(UProjectileMovementComponent* MovementComponent,
	FVector& LocalDirection)
{
	LocalDirection.Normalize();
	FVector WorldDirection = MovementComponent->UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(LocalDirection);
	MovementComponent->Velocity = WorldDirection * MovementComponent->InitialSpeed;
}

void UProjectileShooterComponent::OnShotSpawn(AProjectileActor* SpawnedObject)
{
	OnShotFinished.Broadcast();
}

FVector UProjectileShooterComponent::GetRelativeCharacterForward() const
{
	return GetShooterToTargetDirection();
}

FVector UProjectileShooterComponent::GetRelativeCharacterRight() const
{
	FVector Forward = GetRelativeCharacterForward();
	FVector Up = GetRelativeCharacterUp();
	FVector Right = Up.Cross(Forward);
	return Right;
}

FVector UProjectileShooterComponent::GetRelativeCharacterUp() const
{
	AActor * Actor = GetOwner();
	FTransform Transform = Actor->GetActorTransform();
	FVector Up = Transform.TransformPosition(FVector::UpVector) - Transform.GetLocation();
	Up.Normalize();
	return Up;
}

FVector UProjectileShooterComponent::GetCameraRelativeVector(const FVector& Direction) const
{
	// TODO: Refactor this
	AConfectioniteCharacter* ConfectioniteCharacter = Cast<AConfectioniteCharacter>(GetOwner());
	if (ConfectioniteCharacter == nullptr)
	{
		return FVector::ZeroVector;
	}

	ASCharacterBase* Character = ConfectioniteCharacter->GetAttachedCharacter();
	if (Character == nullptr)
	{
		return FVector::ZeroVector;
	}

	AMyPlayerController* Controller = Cast<AMyPlayerController>(Character->GetController());
	if (Controller == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector CamLoc;
	FRotator CamRot;
	Controller->PlayerCameraManager->GetCameraViewPoint(CamLoc, CamRot);

	const FTransform RotTransform(CamRot);
	return RotTransform.TransformVector(Direction);
}

FVector UProjectileShooterComponent::GetCameraRelativeRight() const
{
	const FVector Vector = GetCameraRelativeVector(FVector::RightVector);
	if (Vector == FVector::ZeroVector)
	{
		return GetRelativeCharacterRight();
	}

	return Vector;
}

FVector UProjectileShooterComponent::GetCameraRelativeUp() const
{
	const FVector Vector = GetCameraRelativeVector(FVector::UpVector);
	if (Vector == FVector::ZeroVector)
	{
		return GetRelativeCharacterUp();
	}

	return Vector;
}


void UProjectileShooterComponent::Initialize(USceneComponent* Origin)
{
	this->ProjectileOrigin = Origin;
}

void UProjectileShooterComponent::SetTarget(USceneComponent* InTarget)
{
	this->Target = InTarget;
	bUseTargetVector = false;
	OnTargetUpdated(InTarget->GetComponentLocation());
}

void UProjectileShooterComponent::SetTargetVector(FVector InTarget)
{
	this->TargetVector = InTarget;
	bUseTargetVector = true;
	this->Target = nullptr;
	OnTargetUpdated(InTarget);
}

void UProjectileShooterComponent::SetHomingTarget(USceneComponent* InTarget)
{
	this->HomingTarget = InTarget;
}

void UProjectileShooterComponent::SetLevel(int32 Level)
{
	CurrentLevel = Level;
}

int32 UProjectileShooterComponent::GetLevel() const
{
	return CurrentLevel;
}

void UProjectileShooterComponent::StartShooting()
{
	IsShooting = true;
	bNextShotAsLast = false;

	if (bPlayLooping)
	{
		GetWorld()->GetTimerManager().SetTimer(ShootingHandle, this, &UProjectileShooterComponent::DoShot, LoopDelay, bPlayLooping);
		if (LoopForSeconds > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(LoopHandle, this, &UProjectileShooterComponent::SetNextShotAsLast, LoopForSeconds, false);
		}
	}

}

void UProjectileShooterComponent::StopShooting()
{
	IsShooting = false;

	bNextShotAsLast = false;
	
	if (ShootingHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootingHandle);
		ShootingHandle.Invalidate();
	}

	if (LoopHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoopHandle);
		LoopHandle.Invalidate();
	}

	OnShootingFinished.Broadcast();
}


// Called every frame
void UProjectileShooterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UProjectileShooterComponent::OnTargetUpdated(const FVector& Direction)
{
}

FVector UProjectileShooterComponent::GetShooterToTargetDirection() const
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

FVector UProjectileShooterComponent::GetShooterToTargetDirection(const FVector& InTarget) const
{
	FVector Direction = FVector::ZeroVector;

	Direction = TargetVector - ProjectileOrigin->GetComponentLocation();
	Direction.Normalize();

	return Direction;
}

FVector UProjectileShooterComponent::GetShooterToTargetDirection(USceneComponent* InTarget) const
{
	FVector Direction = FVector::ZeroVector;
	Direction = InTarget->GetComponentLocation() - ProjectileOrigin->GetComponentLocation();
	Direction.Normalize();
	return Direction;
}


void UProjectileShooterComponent::SetGameplayEffectsUsingContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container)
{
	GameplayEffectsContainer = Container;
}

void UProjectileShooterComponent::ClearEffects()
{
	GameplayEffectsContainer = nullptr;
}

