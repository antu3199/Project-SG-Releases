#include "SGHitboxSubsystem.h"

#include "Components/BoxComponent.h"
#include "SoulsGame/SGConstants.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"

void USGHitboxObject::Init(int32 InHitboxId, const FSGHitboxParams& InParams, UPrimitiveComponent* PrimitiveComponent,
                           bool bShouldOwnComponent)
{
	HitboxId = InHitboxId;
	Params = InParams;

	bEnabled = InParams.bStartEnabled;

	if (bShouldOwnComponent)
	{
		OwnedPrimitiveComponent = PrimitiveComponent;
	}
	else
	{
		ExternalPrimitiveComponent = MakeWeakObjectPtr(PrimitiveComponent);
	}

	if (UPrimitiveComponent* Component = GetPrimitiveComponent())
	{
		if (Component->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision)
		{
			Component->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		}
		
		Component->OnComponentBeginOverlap.AddDynamic(this, &USGHitboxObject::OnOverlapBegin);
		Component->OnComponentEndOverlap.AddDynamic(this, &USGHitboxObject::OnOverlapEnd);
	}
	
}

void USGHitboxObject::SetEnabled(bool Set)
{
	bEnabled = Set;
	if (Set == false)
	{
		LastTriggeredTime = -1;
	}
}

void USGHitboxObject::SetEffectContext(const FSGEffectInstigatorContext& Context)
{
	Params.EffectContext = Context;
}

void USGHitboxObject::RequestDestroy()
{
	if (UPrimitiveComponent* Component = GetPrimitiveComponent())
	{
		Component->OnComponentBeginOverlap.RemoveAll(this);
		Component->OnComponentEndOverlap.RemoveAll(this);
		Component->DestroyComponent();
	}
	
	bEnabled = false;
}

void USGHitboxObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FSGHitboxOverlapContext& OverlapContext = OverlapContexts.Add_GetRef(FSGHitboxOverlapContext());
	OverlapContext.OverlappedComponent = MakeWeakObjectPtr(OverlappedComponent);
	OverlapContext.OtherActor = MakeWeakObjectPtr(OtherActor);
	OverlapContext.OtherComp = MakeWeakObjectPtr(OtherComp);
	CheckOverlap();
}

void USGHitboxObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	int32 Index = -1;
	for (int32 i = 0; i < OverlapContexts.Num(); i++)
	{
		if (OverlapContexts[i].OtherActor == OtherActor && OverlapContexts[i].OtherComp == OtherComp)
		{
			Index = i;
			break;
		}
	}

	if (Index != -1)
	{
		OverlapContexts.RemoveAt(Index);
	}
}

void USGHitboxObject::CheckOverlap()
{
	if (!bEnabled)
	{
		return;
	}
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	bool bHasTriggered = false;
	for (int32 i = 0; i < OverlapContexts.Num(); i++)
	{
		if (LastTriggeredTime == -1 || (CurrentTime - LastTriggeredTime >= Params.TickInterval))
		{
			if (USGHitboxSubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
			{
				FSGHitboxOverlapContext& HitboxOverlapContext = OverlapContexts[i];
				const bool bHasEffect = Params.EffectContext.EffectClass != nullptr && Params.EffectContext.OwnerActor.IsValid();
	
				if (bHasEffect)
				{
					FSGOverlapContext OverlapContext(Params.EffectContext.OwnerActor.Get(), HitboxOverlapContext.OtherActor.Get(), Params.CollisionParams.OverlapBehaviour);
					if (!OverlapContext.ShouldOverlap())
					{
						continue;
					}

					// Apply effect
					if (USGAbilityComponent* AbilityComponent = Params.EffectContext.GetAbilityComponent())
					{
						USGEffect* EffectInstance = AbilityComponent->CreateEffectInstance(Params.EffectContext.EffectClass, Params.EffectContext);
						FSGEffectEvaluationContext EvaluationContext;
						EvaluationContext.TargetActor = HitboxOverlapContext.OtherActor;
						AbilityComponent->GiveEffect(EffectInstance, EvaluationContext);
					}
				}

				HitboxOverlapContext.bAppliedEffect = bHasEffect;

				bHasTriggered = true;
				Subsystem->TriggerHitbox(this, HitboxOverlapContext);
			}
		}
	}

	if (bHasTriggered)
	{
		LastTriggeredTime = CurrentTime;
	}
}

void USGHitboxObject::OnTick(float DeltaTime)
{

#if WITH_EDITOR

	ESGDebugCollisionLevel GlobalDebugLevel = FSGConstants::GetDebugCollisionLevel();

	int32 GlobalDebugLevelValue = static_cast<int32>(GlobalDebugLevel);

	if (UPrimitiveComponent* Component = GetPrimitiveComponent())
	{
		Component->SetVisibility(bEnabled);
		Component->SetHiddenInGame(!bEnabled);
	}

#endif

	if (!bEnabled)
	{
		return;
	}
	
	CheckOverlap();
}

UPrimitiveComponent* USGHitboxObject::GetPrimitiveComponent() const
{
	if (ExternalPrimitiveComponent.IsValid())
	{
		return ExternalPrimitiveComponent.Get();
	}

	if (IsValid(OwnedPrimitiveComponent))
	{
		return OwnedPrimitiveComponent;
	}

	return nullptr;
}

void USGHitboxSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USGHitboxSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USGHitboxSubsystem::Tick(float DeltaTime)
{
	TArray<int32> MarkedForDestroy;
	for (TPair<int32, TObjectPtr<USGHitboxObject>> Pair : HitboxObjects)
	{
		if (!IsValid(Pair.Value))
		{
			MarkedForDestroy.Add(Pair.Key);
			continue;;
		}

		Pair.Value->OnTick(DeltaTime);
	}

	for (const int32 Key : MarkedForDestroy)
	{
		RemoveHitbox(Key);
	}
}

int32 USGHitboxSubsystem::CreateBox(FSGHitboxParams HitboxParams, USceneComponent* ParentComponent, const FString& Socket, const FVector& BoxExtents,
	EAttachmentRule Attachment, const FVector& TranslationOffset, const FRotator& RotationOffset)
{
	if (ParentComponent == nullptr)
	{
		return INDEX_NONE;
	}

	AActor* Actor = ParentComponent->GetOwner();
	if (Actor == nullptr)
	{
		return INDEX_NONE;
	}
	
	UBoxComponent* BoxComponent = NewObject<UBoxComponent>(Actor);
	BoxComponent->SetBoxExtent(BoxExtents);
	BoxComponent->SetRelativeLocation(TranslationOffset);
	BoxComponent->SetRelativeRotation(RotationOffset);
	BoxComponent->RegisterComponent();
	
	const FAttachmentTransformRules AttachmentRules(Attachment, false);
	Actor->AddInstanceComponent(ParentComponent);
	BoxComponent->AttachToComponent(ParentComponent, AttachmentRules, FName(Socket));

	const int32 HitboxId = CreateNewHitboxObject(HitboxParams, BoxComponent, ParentComponent->GetWorld(), true);
	return HitboxId;
}

int32 USGHitboxSubsystem::RegisterExistingComponent(FSGHitboxParams HitboxParams, UPrimitiveComponent* Component)
{
	const int32 HitboxId = CreateNewHitboxObject(HitboxParams, Component, Component->GetWorld(), false);
	return HitboxId;
}

void USGHitboxSubsystem::RemoveHitbox(int32 HitboxId)
{
	if (HitboxObjects.Contains(HitboxId))
	{
		if (USGHitboxObject* HitboxObject = HitboxObjects[HitboxId])
		{
			if (IsValid(HitboxObject))
			{
				HitboxObject->RequestDestroy();
				HitboxObject->MarkAsGarbage();
			}

			HitboxObjects.Remove(HitboxId);

			HandleGenerator.FreeHandle(HitboxId);
		}
	}
}

USGHitboxObject* USGHitboxSubsystem::GetHitbox(int32 HitboxId)
{
	TObjectPtr<USGHitboxObject>* HitboxObject = HitboxObjects.Find(HitboxId);
	if (HitboxObject)
	{
		return *HitboxObject;
	}

	return nullptr;
}

int32 USGHitboxSubsystem::CreateNewHitboxObject(const FSGHitboxParams& Params, UPrimitiveComponent* Component, UWorld* World, bool bOwnsComponent)
{
	const int32 HitboxId = HandleGenerator.GetNewHandle();
	
	USGHitboxObject* NewHitboxObject = NewObject<USGHitboxObject>(World);
	NewHitboxObject->Init(HitboxId, Params, Component, bOwnsComponent);
	HitboxObjects.Add(HitboxId, NewHitboxObject);
	return HitboxId;
}

void USGHitboxSubsystem::TriggerHitbox(USGHitboxObject* HitboxObject, const FSGHitboxOverlapContext& OverlapContext)
{
	OnHitboxTriggered.Broadcast(HitboxObject, OverlapContext);
}

