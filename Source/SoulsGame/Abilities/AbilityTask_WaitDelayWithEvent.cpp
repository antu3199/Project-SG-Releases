// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_WaitDelayWithEvent.h"


#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "TimerManager.h"

UAbilityTask_WaitDelayWithEvent::UAbilityTask_WaitDelayWithEvent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	Time = 0.f;
	TimeStarted = 0.f;
}

UAbilityTask_WaitDelayWithEvent* UAbilityTask_WaitDelayWithEvent::WaitDelayWithEvent(UGameplayAbility* OwningAbility, float Time, FGameplayTagContainer& Tags)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Time);

	UAbilityTask_WaitDelayWithEvent* MyObj = NewAbilityTask<UAbilityTask_WaitDelayWithEvent>(OwningAbility);
	MyObj->Time = Time;
	MyObj->EventTags = Tags;
	return MyObj;
}

void UAbilityTask_WaitDelayWithEvent::ExternalCancel()
{
	Super::ExternalCancel();
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(TimerHandle);
}

void UAbilityTask_WaitDelayWithEvent::Activate()
{
	UWorld* World = GetWorld();
	TimeStarted = World->GetTimeSeconds();

	// Use a dummy timer handle as we don't need to store it for later but we don't need to look for something to clear
	World->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_WaitDelayWithEvent::OnTimeFinish, Time, false);

	const auto OnGameplayEventCallback = FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityTask_WaitDelayWithEvent::OnGameplayEvent);
	EventDelegateHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, OnGameplayEventCallback);
}

void UAbilityTask_WaitDelayWithEvent::OnTimeFinish()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinish.Broadcast();

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventDelegateHandle);
		}
		
	}
	EndTask();
}

void UAbilityTask_WaitDelayWithEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	
}

FString UAbilityTask_WaitDelayWithEvent::GetDebugString() const
{
	float TimeLeft = Time - GetWorld()->TimeSince(TimeStarted);
	return FString::Printf(TEXT("WaitDelay. Time: %.2f. TimeLeft: %.2f"), Time, TimeLeft);
}


