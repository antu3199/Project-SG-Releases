#include "SGProjectilePowerupComponent_SpawnSplineActor.h"

#include "GameFramework/Character.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Character/ConfectioniteCharacter.h"

USGSpawnSplineActorPowerupComponent::USGSpawnSplineActorPowerupComponent()
{
}

void USGSpawnSplineActorPowerupComponent::OnInitializeAbilityContext(USGPowerupAbility* Ability,
	AActor* InAbilitySystemOwner, AActor* InComponentOwner)
{
	Super::OnInitializeAbilityContext(Ability, InAbilitySystemOwner, InComponentOwner);

	bDoPostPowerupImmediately = false;
	LoopDelay = GetLoopDelay(); // Dependent on spline actors
}

void USGSpawnSplineActorPowerupComponent::DoPowerup()
{
	Super::DoPowerup();

	AActor * Owner = GetOwner();

	// TODO: probably good practice to make it not dependent on a cast, but I'm too lazy

	AConfectioniteCharacter* ConfectioniteCharacter = Cast<AConfectioniteCharacter>(Owner);
	if (ConfectioniteCharacter == nullptr)
	{
		return;
	}

	ASCharacterBase* BaseCharacter = ConfectioniteCharacter->GetAttachedCharacter();
	if (BaseCharacter == nullptr)
	{
		return;
	}

	FTransform ComponentTransform = BaseCharacter->GetActorTransform();
	ComponentTransform.SetScale3D(FVector(1,1,1));
	ComponentTransform.AddToTranslation(TransformOffset);

	FVector SpawnLocation;
	const bool bValidLocation = FSUtils::GetGroundLocation(GetWorld(), ComponentTransform.GetLocation(), SpawnLocation);
	if (!bValidLocation)
	{
		return;
	}
	
	ComponentTransform.SetLocation(SpawnLocation);
	
	//Spawn Actor
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Default
	SpawnParameters.bNoFail = true;
	SpawnParameters.Instigator = Cast<APawn>(BaseCharacter);

	ASGSplineActor* SpawnedObject = GetWorld()->SpawnActor<ASGSplineActor>(this->SpawnedSplineActorTemplate, ComponentTransform, SpawnParameters);
	if (SpawnedObject)
	{
		CurrentSpawnedActor = SpawnedObject;
		SpawnedObject->OnPowerupFinishedDelegate.AddDynamic(this, &USGSpawnSplineActorPowerupComponent::OnComponentObjectFinished);

		if (bAttachToParent)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);
			SpawnedObject->AttachToActor(BaseCharacter, AttachmentRules, TEXT(""));
		}

		SpawnedObject->SetGameplayEffectDataContainer(GameplayEffectsContainer);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to spawn object %s"), *GetName());
	}

}

void USGSpawnSplineActorPowerupComponent::StopPowerup()
{
	if (CurrentSpawnedActor != nullptr)
	{
		//CurrentSpawnedActor->Stop();
		//CurrentSpawnedActor->Destroy();
		//CurrentSpawnedActor->DetachFromActor();
		//CurrentSpawnedActor = nullptr;

		CurrentSpawnedActor->OnPowerupFinishedDelegate.RemoveDynamic(this, &USGSpawnSplineActorPowerupComponent::OnComponentObjectFinished);
		CurrentSpawnedActor->SetDestroyOnStop();
	}
}

void USGSpawnSplineActorPowerupComponent::OnComponentObjectFinished()
{
	//Stop();
	OnPostDoPowerup.Broadcast();
}

float USGSpawnSplineActorPowerupComponent::GetLoopDelay()
{
	if (CachedDuration == 0)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Default
		SpawnParameters.bNoFail = true;

		ASGSplineActor* SpawnedObject = GetWorld()->SpawnActor<ASGSplineActor>(this->SpawnedSplineActorTemplate, FTransform::Identity, SpawnParameters);
		CachedDuration = SpawnedObject->GetTotalDuration();
		SpawnedObject->Destroy();
	}

	if (CachedDuration == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cached duration failed!"));
	}
	
	return CachedDuration;
	/*
	ASGSplineActor* CastedObject = Cast<ASGSplineActor>(SpawnedSplineActorTemplate->GetDefaultObject());
	if (!CastedObject->HasInitialized())
	{
		CastedObject->Initialize();
	}
	
	return CastedObject->GetTotalDuration();
	*/
}
