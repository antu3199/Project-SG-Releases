#include "FloatingCombatTextComponent.h"

#include "FloatingTextActor.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "Kismet/GameplayStatics.h"

UFloatingCombatTextComponent::UFloatingCombatTextComponent()
{
}

void UFloatingCombatTextComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFloatingCombatTextComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

void UFloatingCombatTextComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner<APawn>()->IsLocallyControlled() == false)
	{
		return;
	}

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	// Check bad viewport size
	if (ViewportSize.X <= 0.0f || ViewportSize.Y <= 0.0f)
	{
		return;
	}

	// Get the viewport scale (The DPI scale)
	const int32 ViewportX = FGenericPlatformMath::FloorToInt(ViewportSize.X);
	const int32 ViewportY = FGenericPlatformMath::FloorToInt(ViewportSize.Y);
	const float ViewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportX, ViewportY));

	if (ViewportScale <= 0.0f)
	{
		return;
	}

	// Get the owning player's camera location
	const FVector CameraLocation = GetOwner<APawn>()->GetController<APlayerController>()->PlayerCameraManager->GetCameraLocation();

	// ADjust the vertical location of the active text actors by the viewport size and scale so that they appear evenly stacked on the screen
	for (int32 i = 1; i < ActiveTextActors.Num(); i++)
	{
		// Calculate the verticla offset based on how far we (the camera) are from the float text actor
		AFloatingTextActor* TextActor = ActiveTextActors[i];
		const float Distance = FVector::Dist(CameraLocation, TextActor->GetAnchorLocation());
		const float VerticalOffset = Distance / (ViewportSize.X / ViewportScale);

		// Adjust the floating text actor's location based on the calculated amount
		TextActor->SetActorLocation(TextActor->GetAnchorLocation() + FVector(0.0f, 0.0f, i * VerticalOffset * TextVerticalSpacing));
		
	}
	
}

void UFloatingCombatTextComponent::AddFloatingText_Client_Implementation(const FText& Text, FVector WorldLocation, const FColor& Color)
{
	if (FloatingTextActorClass == nullptr)
	{
		return;
	}

	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, WorldLocation + FVector(0.0f, 0.0f, TextVerticalOffset));

	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	// Deferred so we can initialize it before spawning (i.e. Initialize is done before BeginPlay)
	AFloatingTextActor* NewTextActor = World->SpawnActorDeferred<AFloatingTextActor>(FloatingTextActorClass, SpawnTransform, Owner);
	if (NewTextActor == nullptr)
	{
		return;
	}
	// Add the new text actor to the beginning of the active text actors array
	//ActiveTextActors.Insert(NewTextActor, 0);
	ActiveTextActors.Add(NewTextActor);
	
	NewTextActor->Initialize(Text, Color);
	NewTextActor->OnDestroyed.AddDynamic(this, &UFloatingCombatTextComponent::OnTextDestroyed);

	// Need to delegate spawning after to next frame otherwise we get a slate error if spawned on tick
	auto OnFrameFinished = [NewTextActor, SpawnTransform]()
	{
		// Actually spawn actor
		UGameplayStatics::FinishSpawningActor(NewTextActor, SpawnTransform);
	};

	World->GetTimerManager().SetTimerForNextTick(OnFrameFinished);
}

void UFloatingCombatTextComponent::OnTextDestroyed(AActor* DestroyedActor)
{
	int32 DestroyIndex = -1;
	for (int32 i = 0; i < ActiveTextActors.Num(); i++)
	{
		if (ActiveTextActors[i] == DestroyedActor)
		{
			DestroyIndex = i;
			break;
		}
	}

	if (DestroyIndex != -1)
	{
		ActiveTextActors.RemoveAt(DestroyIndex);
	}
}
