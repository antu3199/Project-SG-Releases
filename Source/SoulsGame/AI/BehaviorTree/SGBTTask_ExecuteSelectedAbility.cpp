// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTTask_ExecuteSelectedAbility.h"

#include "SoulsGame/AI/SGAIHelpers.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"
#include "SoulsGame/SGAbilitySystem/AbilityInstances/SGAbility_DashUntilReachTarget.h"

USGBTTask_ExecuteSelectedAbility::USGBTTask_ExecuteSelectedAbility()
{
	NodeName = "ExecuteSelectedAbility";
	bNotifyTick = true;


	// Need to create instance to listen to OnAbilityEnded more easily
	bCreateNodeInstance = true;
}


EBTNodeResult::Type USGBTTask_ExecuteSelectedAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ASGCharacterBase * Character = Cast<ASGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}

	if (Character->GetIsDead())
	{
		return EBTNodeResult::Failed;
	}

	USGAbilityComponent* AbilityComponent = Character->FindComponentByClass<USGAbilityComponent>();
	if (!AbilityComponent)
	{
		return EBTNodeResult::Failed;
	}
	
	USGAIAbilityWrapper * AbilityWrapper = Cast<USGAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		return EBTNodeResult::Failed;
	}

	if (!AbilityWrapper->ExecuteAbilityTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	bAbilityEnded = false;

	CachedOwnerComp = MakeWeakObjectPtr(&OwnerComp);
	

	// TODO: Can we move this the ability wrapper instead?
	//UGameplayAbility* InstancedAbility = nullptr;
	//bool Success = AbilitySystem->InternalTryActivateAbility(AbilityWrapper->AbilitySpecHandle, FPredictionKey(), &InstancedAbility, nullptr, nullptr);
	int32 AbilityHandle = AbilityComponent->RequestActivateAbilityByTag(AbilityWrapper->ExecuteAbilityTag);
	if (AbilityHandle == INDEX_NONE)
	{
		return EBTNodeResult::Failed;
	}

	if (USGAbility* CharacterAbilityInstance = AbilityComponent->GetAbilityInstanceById(AbilityHandle))
	{
		CachedAbility = MakeWeakObjectPtr(CharacterAbilityInstance);
		CachedAbilityHandle = AbilityHandle;
		CharacterAbilityInstance->OnAbilityMontageEnd.AddDynamic(this, &USGBTTask_ExecuteSelectedAbility::OnMontageFinished);


		if (USGAbility_DashUntilReachTarget* DashAbility = Cast<USGAbility_DashUntilReachTarget>(CharacterAbilityInstance))
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


void USGBTTask_ExecuteSelectedAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bAbilityEnded)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void USGBTTask_ExecuteSelectedAbility::EndTask()
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

	// ASCharacterBase * Character = Cast<ASCharacterBase>(CachedOwnerComp->GetAIOwner()->GetPawn());
	// if (!Character)
	// {
	// 	return;
	// }

	// UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	// if (!AbilitySystem)
	// {
	// 	return;
	// }
	
	// if (AbilityEndedHandle.IsValid())
	// {
	// 	AbilitySystem->OnAbilityEnded.Remove(MyMemory->AbilityEndedHandle);
	// 	AbilityEndedHandle.Reset();
	// }
	
	CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, nullptr);
	
}

void USGBTTask_ExecuteSelectedAbility::OnMontageFinished(int32 InstanceHandle)
{
	/*
	if (EventTag != USGAbilitySystemGlobals::GetSG().Tag_AbilityEvent_AbilityBroadcasts_MontageEnd)
	{
		return;
	}
	*/

	if (InstanceHandle != CachedAbilityHandle)
	{
		return;
	}
	
	if (USGAbility* CharacterAbilityInstance = Cast<USGAbility>(CachedAbility))
	{
		CharacterAbilityInstance->OnAbilityMontageEnd.RemoveAll(this);
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
