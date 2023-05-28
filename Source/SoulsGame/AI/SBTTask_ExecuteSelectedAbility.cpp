// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_ExecuteSelectedAbility.h"

#include "SAIHelpers.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/DashUntilReachTargetAbility.h"
#include "SoulsGame/Abilities/PlayMontageCharacterAbility.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"

USBTTask_ExecuteSelectedAbility::USBTTask_ExecuteSelectedAbility()
{
	NodeName = "ExecuteSelectedAbility";
	bNotifyTick = true;


	// Need to create instance to listen to OnAbilityEnded more easily
	bCreateNodeInstance = true;
}


EBTNodeResult::Type USBTTask_ExecuteSelectedAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ASCharacterBase * Character = Cast<ASCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}

	if (Character->GetIsDead())
	{
		return EBTNodeResult::Failed;
	}

	UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	if (!AbilitySystem)
	{
		return EBTNodeResult::Failed;
	}
	
	UAIAbilityWrapper * AbilityWrapper = Cast<UAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		return EBTNodeResult::Failed;
	}

	if (!AbilityWrapper->ExecuteAbilitySpecHandle.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	bAbilityEnded = false;

	CachedOwnerComp = MakeWeakObjectPtr(&OwnerComp);
	AbilitySpecHandle = AbilityWrapper->ExecuteAbilitySpecHandle;
	

	// TODO: Can we move this the ability wrapper instead?
	//UGameplayAbility* InstancedAbility = nullptr;
	//bool Success = AbilitySystem->InternalTryActivateAbility(AbilityWrapper->AbilitySpecHandle, FPredictionKey(), &InstancedAbility, nullptr, nullptr);
	bool Success = AbilitySystem->TryActivateAbility(AbilityWrapper->ExecuteAbilitySpecHandle);

	if (!Success)
	{
		return EBTNodeResult::Failed;
	}

	if (UMyGameplayAbility* CharacterAbilityInstance = Cast<UMyGameplayAbility>(AbilityWrapper->GetMostRecentAbilityInstance(AbilitySystem)))
	{
		CachedAbility = MakeWeakObjectPtr(CharacterAbilityInstance);
		CharacterAbilityInstance->OnBroadcastAbilityEvent.AddDynamic(this, &USBTTask_ExecuteSelectedAbility::OnMontageFinished);


		if (UDashUntilReachTargetAbility* DashAbility = Cast<UDashUntilReachTargetAbility>(CharacterAbilityInstance))
		{
			if (AbilityWrapper->MaximumActivationRange != -1.0f)
			{
				DashAbility->SetStoppingDistance(AbilityWrapper->MaximumActivationRange);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find gameplay ability"));
	}

	return EBTNodeResult::InProgress;
}

// Unused
void USBTTask_ExecuteSelectedAbility::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle != AbilitySpecHandle)
	{
		return;
	}
	
	EndTask();
}

void USBTTask_ExecuteSelectedAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bAbilityEnded)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void USBTTask_ExecuteSelectedAbility::EndTask()
{
	bAbilityEnded = true;

	if (!CachedOwnerComp.IsValid())
	{
		return;
	}
	
	if (bResetAbility)
	{
		CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, nullptr);
	}

	ASCharacterBase * Character = Cast<ASCharacterBase>(CachedOwnerComp->GetAIOwner()->GetPawn());
	if (!Character)
	{
		return;
	}

	UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	if (!AbilitySystem)
	{
		return;
	}
	
	// if (AbilityEndedHandle.IsValid())
	// {
	// 	AbilitySystem->OnAbilityEnded.Remove(MyMemory->AbilityEndedHandle);
	// 	AbilityEndedHandle.Reset();
	// }
	
	CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, nullptr);
	
}

void USBTTask_ExecuteSelectedAbility::OnMontageFinished(UMyGameplayAbility* Ability, FGameplayTag EventTag,
	FGameplayEventData EventPayload)
{
	if (EventTag != USGAbilitySystemGlobals::GetSG().Tag_AbilityEvent_AbilityBroadcasts_MontageEnd)
	{
		return;
	}
	
	if (UMyGameplayAbility* ActiveAbility = CachedAbility.Get())
	{
		if (ActiveAbility != nullptr && ActiveAbility != Ability)
		{
			return;
		}
	}

	if (UMyGameplayAbility* CharacterAbilityInstance = Cast<UMyGameplayAbility>(Ability))
	{
		CharacterAbilityInstance->OnBroadcastAbilityEvent.RemoveAll(this);
	}
	
	
	EndTask();
}


//void USBTTask_ExecuteSelectedAbility::OnMontageFinished()
//{
//	EndTask();
//}

// void USBTTask_ExecuteSelectedAbility::OnMontageCompleted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
// {
// 	EndTask();
// }
