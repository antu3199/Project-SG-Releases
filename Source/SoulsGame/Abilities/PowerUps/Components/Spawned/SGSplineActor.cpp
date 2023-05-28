#include "SGSplineActor.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetTextLibrary.h"

ASGSplineActor::ASGSplineActor()
{
	bAllowTickBeforeBeginPlay = bTickInEditor;
	PrimaryActorTick.bCanEverTick = true;

	Base = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(Base);


	//Component = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	//Component->SetupAttachment(Base);
}

bool ASGSplineActor::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor && bTickInEditor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ASGSplineActor::PostInitProperties()
{
	Super::PostInitProperties();

}


// Initialize editor only
void ASGSplineActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Initialize();
}

void ASGSplineActor::BeginPlay()
{
	Super::BeginPlay();
	Initialize();

}

void ASGSplineActor::OnComponentFinished(USGSplineComponent* Component)
{
	FinishedComponents.Add(Component);
	if (FinishedComponents.Num() == SGComponents.Num())
	{
		// Complete finish
		Stop();

		if (!bDestroyOnStop && bLoop)
		{
			Start();
		}
	}
}

void ASGSplineActor::Start()
{
	FinishedComponents.Reset();
	for (int32 i = 0; i < RealSplineComponents.Num(); i++)
	{
		SGComponents[i]->Start();
	}
}

void ASGSplineActor::Stop()
{
	for (int32 i = 0; i < RealSplineComponents.Num(); i++)
	{
		SGComponents[i]->Stop();
	}
	FinishedComponents.Reset();

	OnPowerupFinishedDelegate.Broadcast();

	if (bDestroyOnStop)
	{
		Destroy();
	}
}

void ASGSplineActor::SetDestroyOnStop()
{
	bDestroyOnStop = true;
}

float ASGSplineActor::GetTotalDuration() const
{
	
	float MaxDuration = 0;
	for (int32 i = 0; i < RealSplineComponents.Num(); i++)
	{
		if (i >= SGComponents.Num())
		{
			continue;
		}

		MaxDuration = FMath::Max(MaxDuration, SGComponents[i]->GetTotalDuration());
	}

	return MaxDuration;
	
}

bool ASGSplineActor::HasInitialized() const
{
	return bInitialized;
}

void ASGSplineActor::Initialize()
{

	bDestroyOnStop = false;
	bInitialized = true;
	
	FinishedComponents.Reset();
	GetComponents<USplineComponent>(RealSplineComponents);
	GetComponents<USGSplineComponent>(SGComponents);

	RealSplineComponents.Sort( [](auto& A, auto& B)
	{
		return A.GetName() < B.GetName();
	});

	SGComponents.Sort( [](auto& A, auto& B)
	{
		return A.GetName() < B.GetName();
	});
	
	if (RealSplineComponents.Num() != SGComponents.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline components do not match SG components"));
		if (SGComponents.Num() < RealSplineComponents.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("SG Components cannot be less than splien components. Error"));
			return;
		}
	}

	for (int32 i = 0; i < RealSplineComponents.Num(); i++)
	{
		SGComponents[i]->Initialize(MakeWeakObjectPtr(this), MakeWeakObjectPtr(RealSplineComponents[i]));
		//if (bLoop)
		//{
		SGComponents[i]->bLoop = false; // Don't support individual component looping for now...
		//}
		if (bAutoActivateSpline)
		{
			SGComponents[i]->bAutoActivateSpline = false; // Redundant
		}
		
		if (bAutoActivateSpline)
		{
			SGComponents[i]->Start();
		}

		SGComponents[i]->SetCollisionProfileType(CollisionParams);
	}
}

void ASGSplineActor::SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container)
{
	Super::SetGameplayEffectDataContainer(Container);
	for (int32 i = 0; i < RealSplineComponents.Num(); i++)
	{
		SGComponents[i]->SetGameplayEffectDataContainer(Container);
	}
}

