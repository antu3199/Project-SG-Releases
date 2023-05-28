#include "SGPowerupComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"
#include "SoulsGame/Abilities/PowerUps/SGPowerupAbility.h"

USGPowerupComponent::USGPowerupComponent() : Super()
{

}

void USGPowerupComponent::OnInitializeAbilityContext(USGPowerupAbility* Ability, AActor* InAbilitySystemOwner,
	AActor* InComponentOwner)
{
	PowerupAbility = MakeWeakObjectPtr(Ability);
	AbilitySystemOwner = MakeWeakObjectPtr(InAbilitySystemOwner);
	ComponentOwner = MakeWeakObjectPtr(InComponentOwner);
	AbilitySystemComponent = MakeWeakObjectPtr(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InAbilitySystemOwner));
	GameplayEffectsContainer = PowerupAbility.Get()->GameplayEffectsContainer;

	LastShotTime = 0;

	if (!bInitialized)
	{
		OnInitializeOneTime();
	}
	
	bInitialized = true;

	//ActorInfo = InActorInfo;
	//ActivationInfo = InActivationInfo;
	//TriggerEventData = InTriggerEventData;
}

void USGPowerupComponent::OnInitializeOneTime()
{
}


void USGPowerupComponent::OnActivateAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bIsActivated = true;

	if (bStartPowerupOnActivated)
	{
		StartPowerup();
	}
}

void USGPowerupComponent::OnEndAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo& ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bIsActivated = false;
	if (bDetachOnEndAbility)
	{
		this->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		this->UnregisterComponent();
		this->DestroyComponent();
	}
	
	StopPowerup();
}

void USGPowerupComponent::ManualActivateAbility()
{
	if (PowerupAbility.IsValid())
	{
		if (UCharacterAbilitySystemComponent* Component = Cast<UCharacterAbilitySystemComponent>(PowerupAbility->GetActorInfo().AbilitySystemComponent))
		{
			FGameplayTag AbilityTag = PowerupAbility->AbilityTags.First();
			Component->ActivateAbilityWithTag(AbilityTag);
		}
	}
}

void USGPowerupComponent::ManualCancel()
{
	StopPowerup();
	/*
	if (PowerupAbility.Get())
	{
		PowerupAbility->K2_CancelAbility();
	}
	else
	{
		OnCancelAbility(nullptr, FGameplayAbilityActivationInfo(), false);
	}
	*/
}

bool USGPowerupComponent::GetDetachOnEndAbility() const
{
	return bDetachOnEndAbility;
}

int32 USGPowerupComponent::GetLevel()
{
	return CurrentLevel;
}

void USGPowerupComponent::SetLevel(int32 Level)
{
	CurrentLevel = Level;
}

void USGPowerupComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// TODO: Replicate stats
}

bool USGPowerupComponent::HasInitialized() const
{
	return bInitialized;
}

void USGPowerupComponent::SetTarget(USceneComponent* InTarget)
{
	this->Target = InTarget;
	bUseTargetVector = false;
	OnTargetUpdated(InTarget->GetComponentLocation());
}

void USGPowerupComponent::SetTargetVector(FVector InTarget)
{
	this->TargetVector = InTarget;
	bUseTargetVector = true;
	this->Target = nullptr;
	OnTargetUpdated(InTarget);
}

void USGPowerupComponent::StartPowerup()
{
	bIsActivated = true;
	bIsShooting = true;
	bNextShotAsLast = false;
	double TimeBetweenLastShot = FApp::GetGameTime() - LastShotTime;
	double StartDelay = 0;

	if (LastShotTime == 0)
	{
		StartDelay = 0;
	}
	else if (TimeBetweenLastShot < LoopDelay)
	{
		StartDelay = LoopDelay - TimeBetweenLastShot;
	}

	GetWorld()->GetTimerManager().SetTimer(ShootingHandle, this, &USGPowerupComponent::RequestPowerup, LoopDelay, bPlayLooping, StartDelay);
	if (LoopForSeconds > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(LoopHandle, this, &USGPowerupComponent::SetNextShotAsLast, LoopForSeconds, false, StartDelay);
	}
}

void USGPowerupComponent::RequestPowerup()
{
	if (CanDoPowerup())
	{
		PreShot();
		DoPowerup();
		PostShot();
	}
}

void USGPowerupComponent::DoPowerup()
{
}

void USGPowerupComponent::PreShot()
{
	LastShotTime = FApp::GetGameTime();

	OnPreDoPowerup.Broadcast();
}

void USGPowerupComponent::PostShot()
{

	if (bDoPostPowerupImmediately)
	{
		OnPostDoPowerup.Broadcast();
	}
}


void USGPowerupComponent::StopPowerup()
{
	bIsActivated = false;
	if (bIsShooting)
	{
		bIsShooting = false;

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

		OnPowerupFinished.Broadcast();
		LastShotTime = 0;
	}
}

void USGPowerupComponent::SetNextShotAsLast()
{
	bNextShotAsLast = true;

	if (LoopHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoopHandle);
		LoopHandle.Invalidate();
	}
}

bool USGPowerupComponent::CanDoPowerup() const
{
	return true;
}


FVector USGPowerupComponent::GetRelativeCharacterForward() const
{
	const AActor* Actor = GetOwner();
	return Actor->GetActorForwardVector();
}

FVector USGPowerupComponent::GetRelativeCharacterRight() const
{
	const FVector Forward = GetRelativeCharacterForward();
	const FVector Up = GetRelativeCharacterUp();
	const FVector Right = Up.Cross(Forward);
	return Right;
}

FVector USGPowerupComponent::GetRelativeCharacterUp() const
{
	AActor * Actor = GetOwner();
	FTransform Transform = Actor->GetActorTransform();
	FVector Up = Transform.TransformPosition(FVector::UpVector) - Transform.GetLocation();
	Up.Normalize();
	return Up;
}

FVector USGPowerupComponent::GetCameraRelativeVector(const FVector& Direction) const
{
	ASCharacterBase* Character = Cast<ASCharacterBase>(AbilitySystemOwner);
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot get camera relativitiy"))
		return FVector::ZeroVector;
	}

	AMyPlayerController* Controller = Cast<AMyPlayerController>(Character->GetController());
	if (Controller == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot get camera relativitiy"))
		return FVector::ZeroVector;
	}

	FVector CamLoc;
	FRotator CamRot;
	Controller->PlayerCameraManager->GetCameraViewPoint(CamLoc, CamRot);

	const FTransform RotTransform(CamRot);
	return RotTransform.TransformVector(Direction);
}

FVector USGPowerupComponent::GetCameraRelativeRight() const
{
	const FVector Vector = GetCameraRelativeVector(FVector::RightVector);
	if (Vector == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot get camera relativitiy"))
		return GetRelativeCharacterRight();
	}

	return Vector;
}

FVector USGPowerupComponent::GetCameraRelativeUp() const
{
	const FVector Vector = GetCameraRelativeVector(FVector::UpVector);
	if (Vector == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot get camera relativitiy"))
		return GetRelativeCharacterUp();
	}

	return Vector;
}


void USGPowerupComponent::OnTargetUpdated(const FVector& Direction)
{
}

