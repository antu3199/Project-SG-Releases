// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayAbility.h"


#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "GameplayTask.h"
#include "SGAbilitySystemGlobals.h"
#include "SoulsGame/SBlueprintLibrary.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SGConstants.h"
#include "SoulsGame/SHitboxManager.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Animation/HitboxGameplayEventDataObject.h"
#include "SoulsGame/Animation/SendGameplayTagEventNS.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"
#include "SoulsGame/Subsystems/UISubsystem.h"
#include "SoulsGame/UI/HUDWidget.h"

// FGameplayAbilitySpecWrapper
using namespace std::placeholders;


void FGameplayAbilitySpecWrapper::Initialize(UAbilitySystemComponent* InComponent)
{
    CachedAbilitySystem = MakeWeakObjectPtr(InComponent);
}

FGameplayAbilitySpecHandle FGameplayAbilitySpecWrapper::GiveAbility(const TSubclassOf<UMyGameplayAbility>& InAbility)
{
    if (CachedAbilitySystem == nullptr)
    {
        return FGameplayAbilitySpecHandle();
    }

    bHasGivenAbility = true;
    Ability = MakeWeakObjectPtr(InAbility.GetDefaultObject());
    FGameplayAbilityActorInfo* CurrentActorInfo = CachedAbilitySystem->AbilityActorInfo.Get();
    Ability->SetCurrentActorInfo(GameplayAbilitySpecHandle, CachedAbilitySystem->AbilityActorInfo.Get());

    GameplayAbilitySpecHandle = CachedAbilitySystem->GiveAbility(Ability.Get());

    return GameplayAbilitySpecHandle;
}

FGameplayAbilitySpecHandle FGameplayAbilitySpecWrapper::GiveAbilityAndActivateOnce(
    const TSubclassOf<UMyGameplayAbility>& InAbility, const FGameplayEventData* GameplayEventData)
{
    if (CachedAbilitySystem == nullptr)
    {
        return FGameplayAbilitySpecHandle();
    }

    bHasGivenAbility = true;
    Ability = MakeWeakObjectPtr(InAbility.GetDefaultObject());
    FGameplayAbilityActorInfo* CurrentActorInfo = CachedAbilitySystem->AbilityActorInfo.Get();
    
    
    Ability->SetCurrentActorInfo(GameplayAbilitySpecHandle, CachedAbilitySystem->AbilityActorInfo.Get());

    FGameplayAbilitySpec AbilitySpec(Ability.Get());
    GameplayAbilitySpecHandle = CachedAbilitySystem->GiveAbilityAndActivateOnce(AbilitySpec, GameplayEventData);
    bRemoveAbilityOnFinish = true;

    return GameplayAbilitySpecHandle;
}

bool FGameplayAbilitySpecWrapper::HasTag(const FString &InTagName) const
{
    if (CachedAbilitySystem == nullptr)
    {
        return false;
    }
    
    FGameplayAbilitySpec * Spec = GetAbilitySpec();
    if (!Spec)
    {
        return false;
    }
    
    const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName(InTagName));
    return Spec->Ability->AbilityTags.HasTag(Tag);
}

bool FGameplayAbilitySpecWrapper::HasTag(const FGameplayTag& InTag) const
{
    FGameplayAbilitySpec * Spec = GetAbilitySpec();
    if (!Spec)
    {
        return false;
    }
    
    return Spec->Ability->AbilityTags.HasTag(InTag);
}

bool FGameplayAbilitySpecWrapper::HasAllTags(const FGameplayTagContainer& HasAllTags) const
{
    FGameplayAbilitySpec * Spec = GetAbilitySpec();
    if (!Spec)
    {
        return false;
    }
    
    return Spec->Ability->AbilityTags.HasAll(HasAllTags);
}

FString FGameplayAbilitySpecWrapper::GetAbilityName() const
{
    FGameplayAbilitySpec * Spec = GetAbilitySpec();
    
    if (Spec == nullptr || Spec->Ability == nullptr)
    {
        return "GenericAbility";
    }
    
    return Spec->Ability->AbilityTags.ToString();
}

bool FGameplayAbilitySpecWrapper::ActivateAbility() const
{
    if (!CachedAbilitySystem.IsValid())
    {
        return false;
    }

    if (!bHasGivenAbility)
    {
        UE_LOG(LogTemp, Warning, TEXT("Warning: %s You haven't given this ability yet but you're trying to activate it"), *GetAbilityName());
    }

    return CachedAbilitySystem->TryActivateAbility(GameplayAbilitySpecHandle);
}

FGameplayAbilitySpec* FGameplayAbilitySpecWrapper::GetAbilitySpec() const
{
    if (!CachedAbilitySystem.IsValid()) return nullptr;
    if (!GameplayAbilitySpecHandle.IsValid()) return nullptr;
    
    return CachedAbilitySystem->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
}

UGameplayAbility* FGameplayAbilitySpecWrapper::GetPrimaryInstancedAbility(int Index) const
{
    FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
    if (!AbilitySpec)
    {
        return nullptr;
    }

    // Get 
    if (AbilitySpec->NonReplicatedInstances.Num() > 0)
    {
        return AbilitySpec->NonReplicatedInstances[Index];
    }
    if (AbilitySpec->ReplicatedInstances.Num() > 0)
    {
        return AbilitySpec->ReplicatedInstances[Index];
    }

    
    // This only returns if instanced per actor
    //return AbilitySpec->GetPrimaryInstance();

    return nullptr;
}

UGameplayAbility* FGameplayAbilitySpecWrapper::GetMostRecentInstancedAbility() const
{
    FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
    if (!AbilitySpec)
    {
        return nullptr;
    }

    // Get 
    if (AbilitySpec->NonReplicatedInstances.Num() > 0)
    {
        return AbilitySpec->NonReplicatedInstances[AbilitySpec->NonReplicatedInstances.Num() - 1];
    }
    if (AbilitySpec->ReplicatedInstances.Num() > 0)
    {
        return AbilitySpec->ReplicatedInstances[AbilitySpec->ReplicatedInstances.Num() - 1];
    }

    
    // This only returns if instanced per actor
    //return AbilitySpec->GetPrimaryInstance();

    return nullptr;
}

// FGameplayAbilitySpecWrapper ENDS

void UMyGameplayAbility::PostInitProperties()
{
    Super::PostInitProperties();

    if (!HasAnyFlags(RF_ClassDefaultObject))
    {
        GameplayEffectsContainer = MakeShareable(new FGameplayEffectsWrapperContainer());
    }
}

void UMyGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActorInfo != nullptr)
	{
		OnGiveAbility_BP(*ActorInfo, Spec);
	}
}

TArray<FActiveGameplayEffectHandle> UMyGameplayAbility::ApplyGameplayEffectSpecToTarget(
    const FGameplayEffectSpecHandle EffectSpecHandle, FGameplayAbilityTargetDataHandle TargetData)
{
    return K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, TargetData);
}

UMyGameplayAbility* UMyGameplayAbility::CreateDefaultAbilityWithEffect(UMyGameplayEffect* InEffect,
    const FString& AbilityTag)
{
    
    UMyGameplayAbility * NewAbility = NewObject<UMyGameplayAbility>();
    if (!AbilityTag.IsEmpty())
    {
        const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName(AbilityTag));
        NewAbility->AbilityTags.AddTag(Tag);
    }

    NewAbility->GameplayEffectsContainer->AddEffect(NewAbility, InEffect);
    return NewAbility;
}

void UMyGameplayAbility::SetCurrentActorInfo(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
    Super::SetCurrentActorInfo(Handle, ActorInfo);
}

bool UMyGameplayAbility::IsRangedAbility() const
{
    return AbilityTags.HasTag(USGAbilitySystemGlobals::GetSG().Tag_AbilityType_Ranged);
}

bool UMyGameplayAbility::IsMeleeAbility() const
{
    return AbilityTags.HasTag(USGAbilitySystemGlobals::GetSG().Tag_AbilityType_Melee);
}

bool UMyGameplayAbility::IsSelfAbility() const
{
    return AbilityTags.HasTag(USGAbilitySystemGlobals::GetSG().Tag_AbilityType_Self);
}

float UMyGameplayAbility::GetMinimumRangeToActivate_Legacy() const
{
    if (MinimumRangeToActivate_Legacy == -1.0f)
    {
        if (IsRangedAbility())
        {
            return FSGConstants::Ability_Default_Ranged_MinimumRange;
        }
        else
        {
            return FSGConstants::Ability_Default_Melee_AcceptableRadius;
        }
    }

    return MinimumRangeToActivate_Legacy;
}

float UMyGameplayAbility::GetMoveToAcceptableRadius_Legacy() const
{
    if (MoveToAcceptableRadius_Legacy == -1.0f)
    {
        if (IsRangedAbility())
        {
            return FSGConstants::Ability_Default_Ranged_AcceptableRadius;
        }
        else
        {
            return FSGConstants::Ability_Default_Melee_AcceptableRadius;
        }
    }

    return MoveToAcceptableRadius_Legacy;
}

float UMyGameplayAbility::GetMaxRangeToActivate() const
{
    return MaxRangeToActivate;
}

float UMyGameplayAbility::GetMinRangeToActivate() const
{
    return MinRangeToActivate;
}

int32 UMyGameplayAbility::GetLevel() const
{
    return CurrentLevel;
}

void UMyGameplayAbility::SetLevel(int32 Level)
{
    CurrentLevel = Level;
}

void UMyGameplayAbility::SetOverrideInstigator(AActor* Other)
{
    OverrideInstigator = MakeWeakObjectPtr(Other);
}

void UMyGameplayAbility::BroadcastAbilityEvent(FGameplayTag EventTag, FGameplayEventData EventPayload)
{
	OnBroadcastAbilityEvent.Broadcast(this, EventTag, EventPayload);
}

/*
void UMyGameplayAbility::OnGameplayTaskActivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskActivated(Task);
    UE_LOG(LogTemp, Warning, TEXT("Gameplay ability activated!"));
}
*/
void UMyGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // Don't need to do the default blueprint pipeline.
    //Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
        PreActivateAbility_BP();
        
        DoActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
        DoActivateAbility_BP();
        
        PostActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
        PostActivateAbility_BP();

    }
}

void UMyGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (EventDelegateHandle.IsValid())
    {
        AActor* Owner = GetActorInfo().OwnerActor.Get();
        UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
        if (AbilitySystem != nullptr)
        {
            AbilitySystem->RemoveGameplayEventTagContainerDelegate(EventFilters, EventDelegateHandle);
        }
    }

    EventDelegateHandle.Reset();

	if (AWeaponActor* WeaponActor = GetHumanoidWeaponActor())
	{
		WeaponActor->EndWeaponAttack();
		if (bEmpoweredByAbility)
		{
			WeaponActor->DisableEmpowerWeapon();	
		}
	}
	if (USHitboxManager* HitboxManager = SBlueprintLibrary::GetHitboxManager())
	{
		for (auto& HitboxId : AllHitboxIds)
		{
			HitboxManager->RemoveHitbox(HitboxId);
		}
	}

	AllHitboxIds.Reset();

	if (bDoNotifyEndOnAbilityEnd)
	{
		CallNotifyEnd();
	}
	
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UMyGameplayAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    FGameplayTagContainer* OptionalRelevantTags)
{
    const bool bShouldCommit = Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
    return bShouldCommit;
}

void UMyGameplayAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    ActivationHandle = Handle;
    ActivationActivationInfo = ActivationInfo;
    ActivationTriggerEventData = TriggerEventData;
    
    if (bListenToEvents)
    {
        AActor* Owner = GetActorInfo().OwnerActor.Get();
        UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
        if (AbilitySystem != nullptr)
        {
            const auto OnGameplayEventCallback = FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UMyGameplayAbility::OnGameplayEvent);
            EventDelegateHandle = AbilitySystem->AddGameplayEventTagContainerDelegate(EventFilters, OnGameplayEventCallback);
        }
    }
    
    this->GameplayEffectsContainer->Reset();
    this->InitializeEffectContainerHelper();
    this->GameplayEffectsContainer->OnBeforeApplyEffectDelegate.BindUObject(this, &UMyGameplayAbility::OnBeforeEffectApply);
    this->GameplayEffectsContainer->OnAfterApplyEffectDelegate.BindUObject(this, &UMyGameplayAbility::OnAfterEffectApply);

	AllHitboxIds.Reset();
	bEmpoweredByAbility = false;

	if (bClearQueuedAbilityOnBegin)
	{
		if (const APawn * Pawn = Cast<APawn>(ActorInfo->OwnerActor))
		{
			if (AMyPlayerController* Controller = Cast<AMyPlayerController>(Pawn->GetController()))
			{
				Controller->ClearQueuedAction();
			}
		}
	}

	
}

void UMyGameplayAbility::DoActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
}

void UMyGameplayAbility::PostActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
}

// Effect functions ====
void UMyGameplayAbility::ApplyEffect(int32 EffectIndex)
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->ApplyEffect(EffectIndex);
}

void UMyGameplayAbility::AddTargetToEffect(int32 EffectIndex, AActor* TargetActor)
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->AddTargetToEffect(EffectIndex, TargetActor);
}

bool UMyGameplayAbility::CanApplyEffectToActor(int32 EffectIndex, AActor* TargetActor)
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return false;
    }

    return this->GameplayEffectsContainer->CanApplyEffectToActor(EffectIndex, TargetActor);
}

int32 UMyGameplayAbility::GetEffectIndexForTag(FGameplayTag Tag) const
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return -1;
    }
    
    return this->GameplayEffectsContainer->GetEffectIndexForTag(Tag);
}

void UMyGameplayAbility::ResetEffectTargets()
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->ResetTargets();
}

void UMyGameplayAbility::SetEffectLevel(int32 EffectIndex, int32 Level)
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->SetEffectLevel(EffectIndex, Level);
}

void UMyGameplayAbility::GetAllActiveGameplayEffects(TSet<FActiveGameplayEffectHandle>& OutEffects) const
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->GetAllActiveGameplayEffects(OutEffects);
}

void UMyGameplayAbility::RemoveActiveGameplayEffectHandle(FActiveGameplayEffectHandle Handle)
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->RemoveActiveGameplayEffectHandle(Handle);
}

void UMyGameplayAbility::SetCustomDamageCauser(AActor* Causer)
{
    if (!this->GameplayEffectsContainer.IsValid())
    {
        return;
    }

    this->GameplayEffectsContainer->SetCustomDamageCauser(Causer);
}

void UMyGameplayAbility::CallNotifyEnd()
{
	ACharacter* PawnCharacter = Cast<ACharacter>(GetActorInfo().OwnerActor.Get());
	if (PawnCharacter == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = PawnCharacter->GetMesh();
	if (!SkeletalMeshComponent)
	{
		return;
	}
	
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	TArray<FAnimNotifyEvent>& Events = AnimInstance->ActiveAnimNotifyState;
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		const FAnimNotifyEvent& AnimNotifyEvent = Events[Index];
		const FAnimNotifyEventReference& EventReference = AnimInstance->ActiveAnimNotifyEventReference[Index];
		if (AnimNotifyEvent.NotifyStateClass && AnimInstance->ShouldTriggerAnimNotifyState(AnimNotifyEvent.NotifyStateClass))
		{
#if WITH_EDITOR
			// Prevent firing notifies in animation editors if requested 
			if(AnimNotifyEvent.NotifyStateClass->ShouldFireInEditor())
#endif
			{
				FAnimTrace::OutputAnimNotify(AnimInstance, AnimNotifyEvent, FAnimTrace::ENotifyEventType::End);
				AnimNotifyEvent.NotifyStateClass->NotifyEnd(SkeletalMeshComponent, Cast<UAnimSequenceBase>(AnimNotifyEvent.NotifyStateClass->GetOuter()), EventReference);
			}
		}
	}



}

// Effect functions END ====

void UMyGameplayAbility::EndAbility(const bool WasCancelled)
{
    this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, WasCancelled);
}

void UMyGameplayAbility::InitializeEffectContainerHelper()
{
    this->GameplayEffectsContainer->Reset();
    GameplayEffectsContainer->Initialize(GetActorInfo().OwnerActor.Get());

    for (TSubclassOf<UMyGameplayEffect> & Effect : this->AppliedGameplayEffects)
    {
        this->GameplayEffectsContainer->AddEffect(this, Effect.GetDefaultObject(), OverrideDuration);
    }
}

bool UMyGameplayAbility::ShouldUseAbility()
{
    FGameplayAbilityActorInfo ActorInfo = GetActorInfo();

    if (ActorInfo.OwnerActor == nullptr)
    {
        return false;
    }
    
    const ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(ActorInfo.OwnerActor);
    if (!CharacterBase)
    {
        return false;
    }

    return CharacterBase->GetIsDead();
}


void UMyGameplayAbility::OnBeforeEffectApply(FGameplayEffectSpecWrapper& Data, TArray<TWeakObjectPtr<AActor>>& HitActors)
{
}

void UMyGameplayAbility::OnAfterEffectApply(FGameplayEffectSpecWrapper& Data, TArray<TWeakObjectPtr<AActor>>& HitActors)
{
}

bool UMyGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    FGameplayTagContainer* OptionalRelevantTags) const
{
    if (bBlueprintOverrideCost && ActorInfo != nullptr)
    {
        return CheckCost_BP(*ActorInfo);
    }
	
    return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}


void UMyGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (bBlueprintOverrideCost && ActorInfo != nullptr)
    {
        ApplyCost_BP(*ActorInfo);
        return;
    }
    
    Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

void UMyGameplayAbility::OnGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
    HandleGameplayEvent(EventTag, Payload);
    FGameplayEventData BP_PayLoad = Payload ? *Payload : FGameplayEventData();
    HandleGameplayEvent_BP(EventTag, BP_PayLoad);

}

bool UMyGameplayAbility::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	// For simultanous events, make sure we're referring to the correct one.
	if (Payload != nullptr && Payload->EventTag.IsValid())
	{
		if (!this->AbilityTags.HasTag(Payload->EventTag))
		{
			return false;
		}
	}
	
	FString Tag = EventTag.ToString();
	
	bool bEventHandled = false;
	
	// Access common trigger
	// Need to make sure to check in reverse order of depth
	if (ASHumanoidPlayerCharacter* HumanoidPlayerCharacter = GetHumanoidPlayerCharacter())
	{
	}
	
	if (ASHumanoidCharacter* PlayerCharacter = GetHumanoidCharacter())
	{
		// Generic objects not bound by tag:
		if (Payload->OptionalObject != nullptr)
		{
			if (const USGSetEffectLevel_GameplayEventDataObject* DataObject = Cast<USGSetEffectLevel_GameplayEventDataObject>(Payload->OptionalObject))
			{
				if (GameplayEffectsContainer.IsValid())
				{
					GameplayEffectsContainer->SetEffectLevel(DataObject->EffectIndex, DataObject->Level);
				}
			}
		}
		
		if (Tag == "Triggers.Character.EnableWeaponHitbox")
		{
			if (AWeaponActor* WeaponActor = GetHumanoidWeaponActor())
			{
				WeaponActor->SetGameplayEffectDataContainer(GameplayEffectsContainer);
				WeaponActor->BeginWeaponAttack();
			}
			
			bEventHandled = true;
		}
		else if (Tag == "Triggers.Character.DisableWeaponHitbox")
		{
			if (AWeaponActor* WeaponActor = GetHumanoidWeaponActor())
			{
				WeaponActor->SetGameplayEffectDataContainer(GameplayEffectsContainer);
				WeaponActor->EndWeaponAttack();
			}
			
			bEventHandled = true;
		}
		else if (Tag == USGAbilitySystemGlobals::GetSG().Tag_Triggers_Character_CheckEmpowered_Enable.ToString())
		{
			if (AWeaponActor* WeaponActor = GetHumanoidWeaponActor())
			{
				bEmpoweredByAbility = WeaponActor->CheckEmpowerWeapon();	
			}
			
			bEventHandled = true;
		}
		else if (Tag == USGAbilitySystemGlobals::GetSG().Tag_Triggers_Character_CheckEmpowered_Disable.ToString())
		{
			if (AWeaponActor* WeaponActor = GetHumanoidWeaponActor())
			{
				WeaponActor->DisableTimestopIfEmpoweredWeapon();
			}
			
			bEventHandled = true;
		}
	
	}
	
	if (ASCharacterBase* Character = GetCharacter())
	{
		if (Tag == "Triggers.Character.ApplyEffectsToSelf")
		{
			// TODO: Allow effect index parameter
			this->GameplayEffectsContainer->AddTargetToEffect(0, GetOwningActorFromActorInfo());
			this->GameplayEffectsContainer->ApplyEffect(0);
			bEventHandled = true;
		}
		else if (Tag == "Triggers.Character.SpawnProjectile")
		{
			const UBlueprint * AbilityActorBP = Cast<UBlueprint>(Payload->OptionalObject);
		
			if (!AbilityActorBP)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid projectile!"));
				return false;
			}
			TSubclassOf<AProjectileActor> GenClass = (TSubclassOf<AProjectileActor>)AbilityActorBP->GeneratedClass;
	    
			const FRotator Rotation = Character->GetActorRotation();
			const FVector Location = Character->GetActorLocation();
			const FVector Scale = FVector::OneVector;
			const FTransform Transform(Rotation,Location, Scale);
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Default
			SpawnParameters.Instigator = Cast<APawn>(Character);
			AAbilityActor * SpawnedObject = GetWorld()->SpawnActor<AAbilityActor>(GenClass, Transform, SpawnParameters);
			if (SpawnedObject)
			{
				SpawnedObject->SetGameplayEffectDataContainer(this->GameplayEffectsContainer);
				// Do something
			}
			bEventHandled = true;
		}
		else if (Tag == "Triggers.Character.DisableRootMotionVelocityOnHit")
		{
			Character->SetStopRootMotionOnHit(true);
			bEventHandled = true;
		}
		else if (Tag == "Triggers.Character.EnableRootMotionVelocityOnHit")
		{
			Character->SetStopRootMotionOnHit(false);
			bEventHandled = true;
		}
		else if (Tag == "Triggers.Character.SetHitboxEffect")
		{
			if (const USGCreateHitbox_GameplayEventDataObject* DataObject = Cast<USGCreateHitbox_GameplayEventDataObject>(Payload->OptionalObject))
			{
				if (USHitboxManager* HitboxManager = SBlueprintLibrary::GetHitboxManager())
				{
					HitboxManager->AddOnOverlapBeginCallback(DataObject->HitboxId, std::bind(&UMyGameplayAbility::OnDynamicHitboxBeginOverlap, this, _1, _2, _3, _4, _5, _6));
					HitboxManager->AddOnOverlapEndCallback(DataObject->HitboxId, std::bind(&UMyGameplayAbility::OnDynamicHitboxEndOverlap, this, _1, _2, _3, _4));
					HitboxManager->AddOnOverlapTickCallback(DataObject->HitboxId, std::bind(&UMyGameplayAbility::OnDynamicHitboxTickOverlap, this, _1, _2), DataObject->OverlapInterval);
	
					AllHitboxIds.Add(DataObject->HitboxId);
				}
			}
	
			bEventHandled = true;
		}


	
	}
	
	return bEventHandled;
}

void UMyGameplayAbility::OnDynamicHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UMyGameplayAbility::OnDynamicHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void UMyGameplayAbility::OnDynamicHitboxTickOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor)
{
	// TODO: Allow for index

	if (USHitboxManager* HitboxManager = SBlueprintLibrary::GetHitboxManager())
	{
		// Make sure overlapped component is the one we're listening to
		const int32 Id = HitboxManager->GetHitboxIdFromComponent(OverlappedComponent);
		if (Id == -1 || !AllHitboxIds.Contains(Id))
		{
			return;
		}
	}

	if (GameplayEffectsContainer->CanApplyEffectToActor(0, OtherActor))
	{
		GameplayEffectsContainer->AddTargetToEffect(0, OtherActor);
		GameplayEffectsContainer->ApplyEffect(0);
	}

	GameplayEffectsContainer->ResetTargets();
}

ASCharacterBase* UMyGameplayAbility::GetCharacter() const
{
	if (CurrentActorInfo == nullptr)
	{
		return nullptr;
	}

	
	return Cast<ASCharacterBase>(GetActorInfo().OwnerActor);
}

ASHumanoidCharacter* UMyGameplayAbility::GetHumanoidCharacter() const
{
	if (CurrentActorInfo == nullptr)
	{
		return nullptr;
	}

	return Cast<ASHumanoidCharacter>(GetActorInfo().OwnerActor);
}

AWeaponActor* UMyGameplayAbility::GetHumanoidWeaponActor() const
{
	if (ASHumanoidCharacter* Character = GetHumanoidCharacter())
	{
		return Character->WeaponActor;
	}

	return nullptr;
}

ASHumanoidPlayerCharacter* UMyGameplayAbility::GetHumanoidPlayerCharacter() const
{
	return Cast<ASHumanoidPlayerCharacter>(GetActorInfo().OwnerActor);
}


