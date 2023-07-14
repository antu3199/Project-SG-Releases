#include "SGAbility.h"

#include "GameplayTagsManager.h"
#include "SGAbilityComponent.h"
#include "SGAbilityTask.h"
#include "SoulsGame/Character/SGHumanoidCharacter.h"

USGAbility* USGAbility::NewSGAbility(UObject* Outer, TSubclassOf<USGAbility> Ability, int32 InstanceHandle, AActor* Owner, AActor* AvatarActor, int32 Level)
{
	USGAbility* NewObj = NewObject<USGAbility>(Outer, Ability);
	NewObj->InstancedAbilityHandle = InstanceHandle;
	NewObj->bIsCDO = false;
	// Avatar actor is always owner unless otherwise specified
	NewObj->ActorInfo.InitFromActor(Owner, AvatarActor ? AvatarActor : Owner);
	NewObj->Level = Level;

	NewObj->OnGiveAbility();
	return NewObj;
}

bool USGAbility::CanActivate_Implementation(const USGAbilityComponent* RequestedComponent)
{
	if (!CreateInstanceOnActivation)
	{
		if (RequestedComponent->IsAbilityActive(this->IdentifierTag))
		{
			return false;
		}
	}
	
	return CheckCost(RequestedComponent);
}

FSGEffectInstigatorContext USGAbility::CreateEffectInstigatorContextForAbility_Implementation(FGameplayTag ContextTag)
{
	FSGEffectInstigatorContext Context;
	Context.OwnerActor = ActorInfo.OwnerActor;
	Context.AvatarActor = ActorInfo.AvatarActor;
	
	return Context;
}

void USGAbility::RequestEndAbility()
{
	if (!bIsActive)
	{
		// Probably already requested end ability
		return;
	}

	if (bRequestCooldownOnAbilityEnd)
	{
		RequestCooldownStart();
	}
	
	OnEndAbility();
	bIsActive = false;
	
	if (USGAbilityComponent* AbilityComponent = ActorInfo.GetAbilityComponent())
	{
		AbilityComponent->OnAbilityInstanceEnded(this);
	}

	OnAbilityEnd.Broadcast();
}

void USGAbility::RequestCooldownStart()
{
	ensure(!bIsCDO && ActorInfo.OwnerActor.IsValid());

	if (USGAbilityComponent* AbilityComponent = ActorInfo.GetAbilityComponent())
	{
		AbilityComponent->RequestCooldownStart(GetIdentifierTag());
	}
}

void USGAbility::RequestStartDurationTimer()
{
	if (Duration == -1.0f)
	{
		OnDurationTimerExpired();
		return;
	}

	check(!DurationTimerHandle.IsValid());

	GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, &USGAbility::OnDurationTimerExpired, Duration, false, Duration);;
}

void USGAbility::SetCurrentMontage(UAnimMontage* InMontage)
{
	Montage = InMontage;
}

void USGAbility::ApplyCost_Implementation()
{

}

bool USGAbility::CheckCost_Implementation(const USGAbilityComponent* RequestedComponent)
{
	if (Cooldown != 0.0f)
	{
		// Check cooldown
		return !RequestedComponent->IsUnderCooldown(GetIdentifierTag());
	}
	
	return true;
}

bool USGAbility::RequestActivate_Implementation()
{
	if (ApplyCostOnActivate)
	{
		if (CheckCost(ActorInfo.GetAbilityComponent()))
		{
			ApplyCost();
			DoActivate();
			return true;
		}
		else
		{
			return false;
		}
	}

	DoActivate();
	return true;
}

void USGAbility::OnActivate_Implementation()
{

}

bool USGAbility::IsUnderCooldown() const
{
	ensureMsgf(!bIsCDO, TEXT("Should not call GetCurrentCooldown on CDO. Use AbilityComponnet version instead!"));

	if (Cooldown == 0.0f)
	{
		return false;
	}

	if (GetCurrentCooldown() > 0.0f)
	{
		return true;
	}

	return false;
}

float USGAbility::GetCurrentCooldown() const
{
	ensureMsgf(!bIsCDO, TEXT("Should not call GetCurrentCooldown on CDO. Use AbilityComponnet version instead!"));

	if (USGAbilityComponent* Component = ActorInfo.GetAbilityComponent())
	{
		return Component->GetCurrentCooldownTime(GetIdentifierTag());
	}

	return 0.0f;
	

}

float USGAbility::GetAbilityActivationCooldown() const
{
	return Cooldown;
}

void USGAbility::SetAvatarActor(AActor* Other)
{
	if (Other == nullptr)
	{
		// Do not allow null avatar actors!
		return;
	}

	ActorInfo.AvatarActor = MakeWeakObjectPtr(Other);
}

UGameplayTasksComponent* USGAbility::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return ActorInfo.GetAbilityComponent();
}

AActor* USGAbility::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return ActorInfo.OwnerActor.Get();
}

AActor* USGAbility::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return ActorInfo.AvatarActor.Get();
}

void USGAbility::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	USGAbilityTask* AbilityTask = Cast<USGAbilityTask>(&Task);
	if (AbilityTask)
	{
		AbilityTask->SetAbilityComponent(ActorInfo.GetAbilityComponent());
		AbilityTask->Ability = this;
	}
}

void USGAbility::OnGameplayTaskActivated(UGameplayTask& Task)
{
	ActiveTasks.Add(&Task);
}

void USGAbility::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	ActiveTasks.Remove(&Task);
}

float USGAbility::GetMaxRangeToActivate() const
{
	return MaxRangeToActivate;
}

float USGAbility::GetMinRangeToActivate() const
{
	return  MinRangeToActivate;
}

AActor* USGAbility::SpawnActorFromClass(TSubclassOf<AActor> ActorClass,
	FVector Location, FRotator Rotation)
{
	if (UWorld* World = GetWorld())
	{
		const FTransform Transform(Rotation,Location, FVector::OneVector);
		const FActorSpawnParameters SpawnParams;
		AActor* NewlySpawnedActor = World->SpawnActor<AActor>(ActorClass, Transform, SpawnParams);
		return NewlySpawnedActor;
	}

	return nullptr;
}

UWorld* USGAbility::GetWorld() const
{
	if (bIsCDO)
	{
		return nullptr;
	}

	return GetOuter()->GetWorld();
}

void USGAbility::OnGiveAbility_Implementation()
{
}

void USGAbility::OnDurationTimerExpired_Implementation()
{
	DurationTimerHandle.Invalidate();

	if (bEndAbilityOnDurationExpired)
	{
		RequestEndAbility();
	}
}

void USGAbility::DoActivate()
{
	bIsActive = true;
	OnActivate();
	if (bRequestCooldownOnActivate)
	{
		RequestCooldownStart();
		checkf(Duration == -1.0f, TEXT("Duration should not be set if we request cooldown on activate!"));
	}

	if (Duration != -1.0f && bStartDurationTimerOnActivate)
	{
		check(Duration != -1.0f);
		RequestStartDurationTimer();
	}
}

float USGAbility::GetLastActivationTime() const
{
	ensureMsgf(!bIsCDO, TEXT("Should not call GetCurrentCooldown on CDO. Use AbilityComponnet version instead!"));

	if (const USGAbilityComponent* Component = ActorInfo.GetAbilityComponent())
	{
		return Component->GetLastActivationTime(GetIdentifierTag());
	}

	return 0.0f;
}

void USGAbility::SetLevel(int32 InLevel)
{
	Level = InLevel;
}

int32 USGAbility::GetLevel() const
{
	return Level;
}

void USGAbility::BroadcastOnAbilityMontageEnd()
{
	OnAbilityMontageEnd.Broadcast(InstancedAbilityHandle);
}

void USGAbility::OnHandleEvent_Implementation(const FSGAbilityEventData& Payload)
{
	static const FGameplayTag EnableWeaponTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Triggers.Character.EnableWeaponHitbox"), false);
	static const FGameplayTag DisableWeaponTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Triggers.Character.DisableWeaponHitbox"), false);

	const FGameplayTag& Tag = Payload.EventTag;
	
	if (Tag == EnableWeaponTag)
	{

		if (ASGHumanoidCharacter* Character = Cast<ASGHumanoidCharacter>(ActorInfo.OwnerActor))
		{
			if (ASGWeaponActor* WeaponActor = Character->GetWeaponActor())
			{
				// TODO: Reset effects
				//WeaponActor->SetGameplayEffectDataContainer(GameplayEffectsContainer);
				WeaponActor->BeginWeaponAttack();
			}
		}

	}
	else if (Tag == DisableWeaponTag)
	{
		if (ASGHumanoidCharacter* Character = Cast<ASGHumanoidCharacter>(ActorInfo.OwnerActor))
		{
			if (ASGWeaponActor* WeaponActor = Character->GetWeaponActor())
			{
				// TODO: Reset effects
				//WeaponActor->SetGameplayEffectDataContainer(GameplayEffectsContainer);
				WeaponActor->EndWeaponAttack();
			}
		}
	}
}

void USGAbility::OnEndAbility_Implementation()
{
}



