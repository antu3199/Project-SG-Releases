#include "SGSplineComponent.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SGSplineActor.h"
#include "SoulsGame/SHitboxManager.h"
using namespace std::placeholders;

USGSplineComponent::USGSplineComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;
	bHasInitialized = false;
}

void USGSplineComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentTime = 0.0f;
	bHasStarted = false;
	bHasStartedOnce = false;
}

void USGSplineComponent::Start()
{
	if (!bHasInitialized)
	{
		return;
	}
	
	CurrentTime = 0.0f;
	bHasStartedOnce = true;

	auto StartCallback = [&]()
	{
		bHasStarted = true;
		OnAlphaChanged(GetAlpha());
	};

	if (StartDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer( StartTimerHandle, StartCallback, StartDelay, false, StartDelay);
	}
	else
	{
		StartCallback();
	}
}

void USGSplineComponent::Stop()
{
	if (!bHasInitialized)
	{
		return;
	}
	
	DetachExistingComponents();
	bHasStarted = false;
	GetWorld()->GetTimerManager().SetTimer(LoopDelayHandle, this, &USGSplineComponent::OnComplete, EndLoopDelay, false, EndLoopDelay);
}

void USGSplineComponent::OnComplete()
{
	if (BaseActorRef.IsValid())
	{
		BaseActorRef->OnComponentFinished(this);
	}
	else
	{
		if (bLoop)
		{
			Start();
		}
	}
}

void USGSplineComponent::OnAlphaChanged(float NewAlpha)
{
	if (!bHasInitialized)
	{
		return;
	}
	
	if (Component.IsValid())
	{
		const float SplineLength = Component->GetSplineLength();
		const float SplineAlpha = FMath::Lerp(0, SplineLength, NewAlpha);
		const FVector FinalLocation = Component->GetLocationAtDistanceAlongSpline(SplineAlpha, SplineCoordinateSpace);
		const FRotator FinalRotation = Component->GetRotationAtDistanceAlongSpline(SplineAlpha, SplineCoordinateSpace);
		OnSplineLocation(NewAlpha, FinalLocation, FinalRotation);
	}

	OnAlphaChangedBlueprint(NewAlpha);
}

void USGSplineComponent::OnSplineLocation(const float& Alpha, const FVector& Location, const FRotator& Rotation)
{
	if (!bHasInitialized)
	{
		return;
	}
	
	OnSplineLocationBlueprint(Alpha, Location, Rotation);

	const FRotator FinalRot =  UKismetMathLibrary::ComposeRotators(FRotator::MakeFromEuler(StartRotation), Rotation);
	
	if (ParticleSystemTemplate != nullptr)
	{
		if (Alpha == 0.0f)
		{
			DetachExistingComponents();
			// Make
			SpawnedNiagaraSceneComponent = NewObject<USceneComponent>(this);
			SpawnedNiagaraSceneComponent->RegisterComponent();
			SpawnedNiagaraSceneComponent->SetWorldLocation(Location);

			
			FFXSystemSpawnParameters SpawnParameters;
			SpawnParameters.WorldContextObject = GetWorld();
			SpawnParameters.SystemTemplate = ParticleSystemTemplate;
			SpawnParameters.Location = Location;
			SpawnParameters.Rotation = FinalRot;

			SpawnParameters.AttachToComponent = SpawnedNiagaraSceneComponent;
			SpawnParameters.bAutoActivate = true; //bAutoActivateSpline;
		
			SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(SpawnParameters);
			
			if (bSpawnHitbox)
			{
				if (USHitboxManager* HitboxManager = USHitboxManager::GetSingleton())
				{
					ActiveHitboxId = HitboxManager->CreateBox(SpawnedNiagaraSceneComponent, TEXT(""), SplineHitboxExtents, EAttachmentRule::KeepRelative, FVector::ZeroVector, FQuat::Identity);
					if (!bDebugShowActiveHitbox)
					{
						HitboxManager->SetDebugLevel(ActiveHitboxId, ESGDebugCollisionLevel::Normal);
					}
					HitboxManager->AddOnOverlapBeginCallback(ActiveHitboxId, std::bind(&USGSplineComponent::OnOverlapBegin, this, _1, _2, _3, _4, _5, _6));
					HitboxManager->SetCollisionProfile(ActiveHitboxId, CollisionParams);
				}
			}
		}
		else
		{
			if (SpawnedNiagaraSceneComponent != nullptr)
			{
				SpawnedNiagaraSceneComponent->SetWorldLocation(Location);
				if (bApplySplineRotation)
				{
					SpawnedNiagaraComponent->SetWorldRotation(FinalRot);
				}
			}
		}



		if (Alpha >= 1.0f)
		{
			// Destroy
			Stop();
		}

	}
}

float USGSplineComponent::GetNormalizedLinearTime() const
{
	return FMath::Clamp(CurrentTime / Duration, 0.0f, 1.0f);
}

float USGSplineComponent::GetAlpha() const
{
	const float NormalizedLinearTime = GetNormalizedLinearTime();
	double Alpha = UKismetMathLibrary::Ease(0, 1, NormalizedLinearTime, EasingFunction, EasingBlendExp, EasingSteps);
	return Alpha;
}

float USGSplineComponent::GetTotalDuration() const
{
	return StartDelay + Duration + EndLoopDelay;
}

void USGSplineComponent::SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container)
{
	GameplayEffectDataContainer = Container;
}

TWeakPtr<FGameplayEffectsWrapperContainer> USGSplineComponent::GetGameplayEffectsWrapperContainer()
{
	return GameplayEffectDataContainer;
}

void USGSplineComponent::SetCollisionProfileType(FSGCollisionParams InCollisionParams)
{
	CollisionParams = InCollisionParams;
}

void USGSplineComponent::DetachExistingComponents()
{
	if (SpawnedNiagaraSceneComponent != nullptr)
	{
		if (SpawnedNiagaraComponent != nullptr)
		{
			if (bDeactivateNiagaraOnDetatch)
			{
				SpawnedNiagaraComponent->Deactivate();
			}
			
			FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);
			SpawnedNiagaraComponent->DetachFromComponent(DetachmentRules);
			SpawnedNiagaraComponent = nullptr;
		}
		SpawnedNiagaraSceneComponent = nullptr;
	}

	if (ActiveHitboxId != -1)
	{
		if (USHitboxManager* HitboxManager = USHitboxManager::GetSingleton())
		{
			HitboxManager->RemoveHitbox(ActiveHitboxId);
		}
		ActiveHitboxId = -1;
	}
}

void USGSplineComponent::ApplyGameplayEffectsToActor(AActor* Other)
{
	if (GameplayEffectDataContainer == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Apply effect to target: %s"), *Other->GetActorNameOrLabel());

	// TODO: Allow for effect index specification
	GameplayEffectDataContainer.Pin()->AddTargetToEffect(0, Other);
	GameplayEffectDataContainer.Pin()->ApplyEffect(0);
}

void USGSplineComponent::RemoveGameplayEffects(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent || !GameplayEffectDataContainer.IsValid())
	{
		return;
	}
	
	for (auto EffectSpec : GameplayEffectDataContainer.Pin()->Effects)
	{
		for (auto &ActiveHandle : EffectSpec.ActiveGameplayEffectHandles)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveHandle);
		}
	}
}

bool USGSplineComponent::CanApplyEffectToActor(AActor* Other)
{
	if (GameplayEffectDataContainer == nullptr)
	{
		return true; // You can always apply no effect
	}

	// TODO: Allow for effect index specification
	return GameplayEffectDataContainer.Pin()->CanApplyEffectToActor(0, Other);
}

void USGSplineComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanApplyEffectToActor(OtherActor))
	{
		return;
	}
	
	ApplyGameplayEffectsToActor(OtherActor);
}

void USGSplineComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bHasInitialized)
	{
		return;
	}

	if (bHasStarted == false && bAutoActivateSpline && !bHasStartedOnce)
	{
		Start();
		return;
	}

	if (bHasStarted)
	{
		CurrentTime += DeltaTime;
		OnAlphaChanged(GetAlpha());
	}
}

void USGSplineComponent::Initialize(TWeakObjectPtr<ASGSplineActor> BaseActor, TWeakObjectPtr<USplineComponent> Spline)
{
	BaseActorRef = BaseActor;
	Component = Spline;
	bHasInitialized = true;
}

