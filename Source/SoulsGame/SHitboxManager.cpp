#include "SHitboxManager.h"

#include "SGameSingleton.h"
#include "SGConstants.h"
#include "SUtils.h"
#include "Components/MeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

void UHitboxObject::Tick(float DeltaTime)
{
	if (World == nullptr || World.IsStale())
	{
		return;
	}
	
	if (Component == nullptr || !Component->IsValidLowLevel())
	{
		return;
	}

	//Component->TickComponent(DeltaTime, ELevelTick::LEVELTICPK_All, nullptr);
	//Component->ReceiveTick(DeltaTime);
	
	FVector NowLocation = Component->GetComponentLocation();

	//bool bPerformUpdate = FVector::DistSquared(PreviousLocation, NowLocation) > 0.1f;

	bool bPerformUpdate = true;
	
	if (!GIsPlayInEditorWorld && Component->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		bPerformUpdate = false;
	}

#if WITH_EDITOR
	ESGDebugCollisionLevel GlobalDebugLevel = FSGConstants::GetDebugCollisionLevel();

	int32 GlobalDebugLevelValue = static_cast<int32>(GlobalDebugLevel);
	
	if (GlobalDebugLevelValue == 0 || GlobalDebugLevelValue < DebugLevel)
	{
		bPerformUpdate = false;
	}

	// if (FDateTime::Now() - LastUpdateTime <= 0.1f)
	// {
	// 	bPerformUpdate = false;
	// }

	if (bPerformUpdate)
	{

		Component->SetHiddenInGame(false);
	}
	else
	{
		Component->SetHiddenInGame(true);
	}

	if (bPerformUpdate && FDateTime::Now() - LastUpdateTime > 0.1f)
	{
		if (UBoxComponent* Box = Cast<UBoxComponent>(Component))
		{
			FSUtils::DrawBox(World.Get(), Box);
		}
		else if (USphereComponent* Sphere = Cast<USphereComponent>(Component))
		{
			FSUtils::DrawSphere(World.Get(), Sphere);
		}
		else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Component))
		{
			FSUtils::DrawCapsule(World.Get(), Capsule);
		}
	}
#endif

	PreviousLocation = NowLocation;

	if (bIsAbleToDoOverlapTick)
	{
		TArray<AActor*> OverlappingActors;
		Component->GetOverlappingActors(OverlappingActors);
		for (const auto& OverlappedActor : OverlappingActors)
		{

			FSGOverlapContext OverlapContext(Actor.Get(), OverlappedActor, CollisionParams.OverlapBehaviour);
			if (!OverlapContext.ShouldOverlap())
			{
				continue;
			}
		
			if (OnOverlapTickCallback != nullptr)
			{
				// Need to do on next tick so we don't accidentally modify 
				// OnOverlapTickCallback(Component, OverlappedActor);
				GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
				{
					OnOverlapTickCallback(Component, OverlappedActor);
				});

				if (OverlapTickInterval != -1.0f)
				{
					bIsAbleToDoOverlapTick = false;
					World->GetTimerManager().SetTimer(OverlapRefreshHandle, this, &UHitboxObject::RefreshOverlapTick, OverlapTickInterval, false);
				}
				
			}
		}


		
	}


	LastUpdateTime = FDateTime::Now();
}

void UHitboxObject::RemoveFromActor()
{
	if (World == nullptr || World.IsStale())
	{
		return;
	}
	
	if (IsValid(Component))
	{
		Component->OnComponentBeginOverlap.RemoveAll(this);
		Component->OnComponentEndOverlap.RemoveAll(this);

		if (!bExistingComponent)
		{
			Component->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			Component->UnregisterComponent();
			Component->DestroyComponent();
			Component->MarkAsGarbage();
		}
	}

	Component = nullptr;

	if (OverlapRefreshHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(OverlapRefreshHandle);
	}
	// OnOverlapBeginCallback = nullptr;
	// OnOverlapEndCallback = nullptr;
	// OnOverlapTickCallback = nullptr;
}

bool UHitboxObject::IsHitboxValid() const
{
	if (World == nullptr || World.IsStale())
	{
		return false;
	}
	
	if (!IsValid(Component))
	{
		return false;
	}

	return true;
}


void UHitboxObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	
	if (OnOverlapBeginCallback != nullptr)
	{
		FSGOverlapContext OverlapContext(Actor.Get(), OtherActor, CollisionParams.OverlapBehaviour);
		if (!OverlapContext.ShouldOverlap())
		{
			return;
		}
		
		OnOverlapBeginCallback(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void UHitboxObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OnOverlapEndCallback != nullptr)
	{
		FSGOverlapContext OverlapContext(Actor.Get(), OtherActor, CollisionParams.OverlapBehaviour);
		if (!OverlapContext.ShouldOverlap())
		{
			return;
		}
		
		OnOverlapEndCallback(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	}
}

void UHitboxObject::BeginDestroy()
{
	USHitboxManager* Manager = USHitboxManager::GetSingleton();
	if (Manager != nullptr)
	{
		Manager->RemoveHitbox(HitboxId);
	}

	
	UObject::BeginDestroy();
}

void UHitboxObject::SetOnOverlapBegin(const FOnOverlapBeginCallback& BeginCallback)
{

	if (Component->OnComponentBeginOverlap.IsBound())
	{
		Component->OnComponentBeginOverlap.Clear();
		//UE_LOG(LogTemp, Warning, TEXT("Delegate is bound. Fix this!"));
	}
	
	OnOverlapBeginCallback = BeginCallback;

	Component->SetGenerateOverlapEvents(true);
	if (Component->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision)
	{
		Component->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	Component->OnComponentBeginOverlap.AddDynamic(this, &UHitboxObject::OnOverlapBegin);
}

void UHitboxObject::SetOnOverlapEnd(const FOnOverlapEndCallback& EndCallback)
{
	if (Component->OnComponentEndOverlap.IsBound())
	{
		Component->OnComponentEndOverlap.Clear();
		//UE_LOG(LogTemp, Warning, TEXT("Delegate is bound. Fix this!"));
	}
	
	
	OnOverlapEndCallback = EndCallback;

	Component->SetGenerateOverlapEvents(true);
	if (Component->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision)
	{
		Component->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	Component->OnComponentEndOverlap.AddDynamic(this, &UHitboxObject::OnOverlapEnd);
}

void UHitboxObject::SetOnOverlapTick(const FOnOverlapTickCallback& TickCallback, float TickInterval)
{
	OnOverlapTickCallback = TickCallback;
	OverlapTickInterval = TickInterval;

	Component->SetGenerateOverlapEvents(true);
	if (Component->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision)
	{
		Component->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	
	
}


void UHitboxObject::RefreshOverlapTick()
{
	this->bIsAbleToDoOverlapTick = true;
}

USHitboxManager* USHitboxManager::GetSingleton()
{
	if (USGameSingleton * GameSingleton = Cast<USGameSingleton>(GEngine->GameSingleton))
	{
		return GameSingleton->GetHitboxManager();
	}
	
	return nullptr;
}

int32 USHitboxManager::CreateBox(USceneComponent* ParentComponent, const FString& Socket,
                                           const FVector& BoxExtents, EAttachmentRule Attachment, const FVector& TranslationOffset,
                                           const FQuat& RotationOffset)
{
	if (ParentComponent == nullptr)
	{
		return INVALID_HITBOX_ID;
	}

	AActor* Actor = ParentComponent->GetOwner();
	if (Actor == nullptr)
	{
		return INVALID_HITBOX_ID;
	}
	
	UBoxComponent* BoxComponent = NewObject<UBoxComponent>(Actor);
	BoxComponent->SetBoxExtent(BoxExtents);
	BoxComponent->SetRelativeLocation(TranslationOffset);
	BoxComponent->SetRelativeRotation(RotationOffset);
	BoxComponent->RegisterComponent();
	

	const FAttachmentTransformRules AttachmentRules(Attachment, false);
	Actor->AddInstanceComponent(ParentComponent);
	BoxComponent->AttachToComponent(ParentComponent, AttachmentRules, FName(Socket));

	int32 HitboxId = CreateNewHitboxObject(BoxComponent, ParentComponent->GetWorld(), false);
	return HitboxId;
}

int32 USHitboxManager::CreateSphere(USceneComponent* ParentComponent, const FString& Socket, const float& Radius,
	EAttachmentRule Attachment, const FVector& TranslationOffset, const FQuat& RotationOffset)
{
	if (ParentComponent == nullptr)
	{
		return INVALID_HITBOX_ID;
	}

	AActor* Actor = ParentComponent->GetOwner();
	if (Actor == nullptr)
	{
		return INVALID_HITBOX_ID;
	}

	USphereComponent* SphereComponent = NewObject<USphereComponent>(Actor);
	SphereComponent->SetSphereRadius(Radius);
	SphereComponent->SetRelativeLocation(TranslationOffset);
	SphereComponent->SetRelativeRotation(RotationOffset);
	SphereComponent->RegisterComponent();

	const FAttachmentTransformRules AttachmentRules(Attachment, false);
	Actor->AddInstanceComponent(SphereComponent);
	SphereComponent->AttachToComponent(ParentComponent, AttachmentRules, FName(Socket));

	int32 HitboxId = CreateNewHitboxObject(SphereComponent, ParentComponent->GetWorld(), false);

	return HitboxId;
}

int32 USHitboxManager::CreateCapsule(USceneComponent* ParentComponent, const FString& Socket,
	const float& HalfHeight, const float& Radius, EAttachmentRule Attachment, const FVector& TranslationOffset,
	const FQuat& RotationOffset)
{
	if (ParentComponent == nullptr)
	{
		return INVALID_HITBOX_ID;
	}

	AActor* Actor = ParentComponent->GetOwner();
	if (Actor == nullptr)
	{
		return INVALID_HITBOX_ID;
	}

	UCapsuleComponent* CapsuleComponent = NewObject<UCapsuleComponent>(Actor);
	CapsuleComponent->SetCapsuleRadius(Radius);
	CapsuleComponent->SetCapsuleHalfHeight(HalfHeight);
	CapsuleComponent->SetRelativeLocation(TranslationOffset);
	CapsuleComponent->SetRelativeRotation(RotationOffset);
	CapsuleComponent->RegisterComponent();

	const FAttachmentTransformRules AttachmentRules(Attachment, false);
	Actor->AddInstanceComponent(CapsuleComponent);
	CapsuleComponent->AttachToComponent(ParentComponent, AttachmentRules, FName(Socket));

	int32 HitboxId = CreateNewHitboxObject(CapsuleComponent, ParentComponent->GetWorld(), false);


	
	return HitboxId;
}

int32 USHitboxManager::GetHitboxIdFromComponent(UPrimitiveComponent* Component) const
{
	int32 Index = -1;
	for (auto HitboxObjectPair : HitboxObjects)
	{
		Index++;
		if (!IsValid(HitboxObjectPair.Value)|| !HitboxObjectPair.Value->IsHitboxValid())
		{
			continue;
		}
		
		if (HitboxObjectPair.Value->Component == Component)
		{
			return Index; 
		}
	}

	return -1;
}

int32 USHitboxManager::RegisterExistingComponent(UPrimitiveComponent* Component)
{
	int32 HitboxId = CreateNewHitboxObject(Component, Component->GetWorld(), true);
	return HitboxId;
}

void USHitboxManager::Tick(float DeltaTime)
{
	for (auto HitboxObjectPair : HitboxObjects)
	{
		if (!IsValid(HitboxObjectPair.Value)|| !HitboxObjectPair.Value->IsHitboxValid())
		{
			continue;
		}
		
		HitboxObjectPair.Value->Tick(DeltaTime);
	}

}

void USHitboxManager::RemoveHitbox(int32 HitboxId)
{

	if (HitboxObjects.Contains(HitboxId))
	{
		UHitboxObject* HitboxObject = HitboxObjects[HitboxId];

		if (!IsValid(HitboxObject) || !HitboxObject->IsHitboxValid())
		{
			return;
		}
		
		if (IsValid(HitboxObject))
		{
			HitboxObject->RemoveFromActor();
		}

		if (HitboxObject->World != nullptr)
		{
			HitboxObject->MarkAsGarbage();
		}

		UE_LOG(LogTemp, Warning, TEXT("Removed: %s . HitboxId: %d"), *HitboxObject->GetName(), HitboxId);

		HitboxObjects.Remove(HitboxId);

		ReusableHitboxIds.Add(HitboxId);
	}
}


void USHitboxManager::SetCollisionProfile(int32 HitboxId, const FSGCollisionParams& CollisionParams)
{
	if (!HitboxObjects.Contains(HitboxId))
	{
		return;
	}

	UHitboxObject* HitboxObject = HitboxObjects[HitboxId];
	FName CollisionName = FSGCollisionProfileHelper::GetCollisionProfileName(CollisionParams.ProfileType);
	HitboxObject->Component->SetCollisionProfileName(CollisionName);
	HitboxObject->CollisionParams = CollisionParams;
}

void USHitboxManager::AddOnOverlapBeginCallback(int32 HitboxId, FOnOverlapBeginCallback OnOverlapBeginCallback)
{
	if (!HitboxObjects.Contains(HitboxId))
	{
		return;
	}

	UHitboxObject* HitboxObject = HitboxObjects[HitboxId];
	if (!IsValid(HitboxObject) || !HitboxObject->IsHitboxValid())
	{
		return;
	}
	
	HitboxObject->SetOnOverlapBegin(OnOverlapBeginCallback);
}

void USHitboxManager::AddOnOverlapEndCallback(int32 HitboxId, FOnOverlapEndCallback OnOverlapEndCallback)
{
	if (!HitboxObjects.Contains(HitboxId))
	{
		return;
	}

	UHitboxObject* HitboxObject = HitboxObjects[HitboxId];
	if (!IsValid(HitboxObject) || !HitboxObject->IsHitboxValid())
	{
		return;
	}
	
	HitboxObject->SetOnOverlapEnd(OnOverlapEndCallback);
}

void USHitboxManager::AddOnOverlapTickCallback(int32 HitboxId, FOnOverlapTickCallback OnOverlapTickCallback, float TickInterval)
{
	if (!HitboxObjects.Contains(HitboxId))
	{
		return;
	}

	UHitboxObject* HitboxObject = HitboxObjects[HitboxId];
	if (!IsValid(HitboxObject) || !HitboxObject->IsHitboxValid())
	{
		return;
	}
	
	HitboxObject->SetOnOverlapTick(OnOverlapTickCallback, TickInterval);
}

void USHitboxManager::SetDebugLevel(int32 HitboxId, ESGDebugCollisionLevel DebugLevel)
{
	if (!HitboxObjects.Contains(HitboxId))
	{
		return;
	}

	UHitboxObject* HitboxObject = HitboxObjects[HitboxId];
	if (!IsValid(HitboxObject) || !HitboxObject->IsHitboxValid())
	{
		return;
	}
	
	HitboxObject->DebugLevel = static_cast<int32>(DebugLevel);
}


int32 USHitboxManager::CreateNewHitboxObject(UPrimitiveComponent* Component, UWorld* World, bool bExistingComponent)
{
	int32 HitboxId = -1;
	if (!ReusableHitboxIds.IsEmpty())
	{
		int32 Id = *ReusableHitboxIds.begin();
		HitboxId = Id;;
		ReusableHitboxIds.Remove(HitboxId);
	}
	else
	{
		HitboxId = RunningHitboxId++;
	}
	
	Component->SetVisibility(true);

	UHitboxObject* NewHitboxObject = NewObject<UHitboxObject>(World);
	NewHitboxObject->HitboxId = HitboxId;
	NewHitboxObject->Component = Component;
	auto Test = MakeWeakObjectPtr(World);
	NewHitboxObject->World = MakeWeakObjectPtr(World);
	NewHitboxObject->Actor =  MakeWeakObjectPtr(Component->GetOwner());
	NewHitboxObject->PreviousLocation = Component->GetComponentLocation();
	NewHitboxObject->LastUpdateTime = FDateTime::Now();
	NewHitboxObject->bExistingComponent = bExistingComponent;
	
	HitboxObjects.Add(HitboxId, NewHitboxObject);

	UE_LOG(LogTemp, Warning, TEXT("Added: %s . HitboxId: %d"),*NewHitboxObject->GetName(), HitboxId);

	return HitboxId;
}

int32 USHitboxManager::GetHitboxIdFromPrimitiveComponent(UPrimitiveComponent* Component)
{
	if (Component == nullptr)
	{
		return INVALID_HITBOX_ID;
	}
	
	for (int32 i = 0; i < HitboxObjects.Num(); i++)
	{

		if (!IsValid(HitboxObjects[i]) || !HitboxObjects[i]->IsHitboxValid())
		{
			continue;
		}
		
		if (!IsValid(HitboxObjects[i]->Component))
		{
			continue;
		}
		
		if (HitboxObjects[i]->Component == Component)
		{
			return HitboxObjects[i]->HitboxId;
		}
	}

	return INVALID_HITBOX_ID;
}
