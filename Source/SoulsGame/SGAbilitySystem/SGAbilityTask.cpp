#include "SGAbilityTask.h"

#include "SGAbility.h"
#include "SGAbilityComponent.h"

USGAbilityTask::USGAbilityTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bWasSuccessfullyDestroyed = false;
}

void USGAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	bWasSuccessfullyDestroyed = true;
	Super::OnDestroy(bInOwnerFinished);
}

void USGAbilityTask::BeginDestroy()
{
	Super::BeginDestroy();

	
	if (!bWasSuccessfullyDestroyed)
	{
		bWasSuccessfullyDestroyed = true;
	}
}

USGAbility* USGAbilityTask::GetAbility() const
{
	return Ability;
}

void USGAbilityTask::SetAbilityComponent(USGAbilityComponent* InAbilityComponent)
{
	AbilityComponent = InAbilityComponent;
}

bool USGAbilityTask::ShouldBroadcastAbilityTaskDelegates() const
{
	bool ShouldBroadcast = (Ability && Ability->IsActive());

	return ShouldBroadcast;
}

void USGAbilityTask::InitSimulatedTask(UGameplayTasksComponent& GameplayTasksComponent)
{
	Super::InitSimulatedTask(GameplayTasksComponent);

	SetAbilityComponent(Cast<USGAbilityComponent>(TasksComponent.Get()));
}
