#pragma once

// Similar to AbilityTask.h, but with SGAbilityComponent

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTask.h"
#include "Abilities/GameplayAbility.h"
#include "SGAbilityTask.generated.h"

class USGAbility;
class USGAbilityComponent;
class UGameplayTasksComponent;


/**
 *	Latent tasks are waiting on something. This is to differeniate waiting on the user to do something vs waiting on the game to do something.
 *	Tasks start WaitingOnGame, and are set to WaitingOnUser when appropriate (see WaitTargetData, WaitIiputPress, etc)
 */

UCLASS(Abstract)
class SOULSGAME_API USGAbilityTask : public UGameplayTask
{
public:
	GENERATED_BODY()

	USGAbilityTask(const FObjectInitializer& ObjectInitializer);

	virtual void OnDestroy(bool bInOwnerFinished) override;
	virtual void BeginDestroy() override;
	
	USGAbility* GetAbility() const;

	void SetAbilityComponent(USGAbilityComponent* InAbilityComponent);

	/** GameplayAbility that created us */
	UPROPERTY()
	USGAbility* Ability;

	UPROPERTY()
	USGAbilityComponent* AbilityComponent;

	/** This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.  */
	bool ShouldBroadcastAbilityTaskDelegates() const;

	virtual void InitSimulatedTask(UGameplayTasksComponent& GameplayTasksComponent) override;

	/** Helper function for instantiating and initializing a new task */
	template <class T>
	static T* NewSGAbilityTask(USGAbility* ThisAbility, FName InstanceName = FName())
	{
		check(ThisAbility);

		T* MyObj = NewObject<T>();
		MyObj->InitTask(*ThisAbility, FGameplayTasks::DefaultPriority);
		MyObj->InstanceName = InstanceName;
		return MyObj;
	}

	template<typename T>
	static bool DelayedFalse()
	{
		return false;
	}

	// this function has been added to make sure AbilityTasks don't use this method
	template <class T>
	FORCEINLINE static T* NewTask(UObject* WorldContextObject, FName InstanceName = FName())
	{
		static_assert(DelayedFalse<T>(), "UAbilityTask::NewTask should never be used. Use NewAbilityTask instead");
	}

	uint8 bWasSuccessfullyDestroyed : 1;

};

