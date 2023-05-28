// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_ExecuteSelectedAbility_Legacy.h"

#include "SAIHelpers.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SUtils.h"

USBTTask_ExecuteSelectedAbility_Legacy::USBTTask_ExecuteSelectedAbility_Legacy()
{
	NodeName = "ExecuteSelectedAbility";
	bNotifyTick = true;
}


EBTNodeResult::Type USBTTask_ExecuteSelectedAbility_Legacy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	UAIAbilityWrapper_Legacy * AbilityWrapper = Cast<UAIAbilityWrapper_Legacy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		return EBTNodeResult::Failed;
	}

	if (!AbilityWrapper->AbilitySpecHandle.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	CachedNodeMemory = NodeMemory;

	FBTExecuteSelectedAbilityMemory_Legacy* MyMemory = CastInstanceNodeMemory<FBTExecuteSelectedAbilityMemory_Legacy>(NodeMemory);
	MyMemory->bAbilityEnded = false;
	MyMemory->AbilityEndedHandle = AbilitySystem->OnAbilityEnded.AddUObject(this,  &USBTTask_ExecuteSelectedAbility_Legacy::OnAbilityEnded);
	MyMemory->CachedOwnerComp = &OwnerComp;
	MyMemory->AbilitySpecHandle = AbilityWrapper->AbilitySpecHandle;
	
	bool Success = AbilitySystem->TryActivateAbility(AbilityWrapper->AbilitySpecHandle);
	if (!Success)
	{
		return EBTNodeResult::Failed;
	}
	

	return EBTNodeResult::InProgress;
}

void USBTTask_ExecuteSelectedAbility_Legacy::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	FBTExecuteSelectedAbilityMemory_Legacy* MyMemory = CastInstanceNodeMemory< FBTExecuteSelectedAbilityMemory_Legacy>(CachedNodeMemory);
	if (!MyMemory)
	{
		return;
	}

	if (AbilityEndedData.AbilitySpecHandle != MyMemory->AbilitySpecHandle)
	{
		return;
	}
	MyMemory->bAbilityEnded = true;

	if (!MyMemory->CachedOwnerComp)
	{
		return;
	}
	
	if (bResetAbility)
	{
		MyMemory->CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, nullptr);
	}

	ASCharacterBase * Character = Cast<ASCharacterBase>(MyMemory->CachedOwnerComp->GetAIOwner()->GetPawn());
	if (!Character)
	{
		return;
	}

	UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	if (!AbilitySystem)
	{
		return;
	}
	
	if (!MyMemory->AbilityEndedHandle.IsValid())
	{
		return;
	}
	
	AbilitySystem->OnAbilityEnded.Remove(MyMemory->AbilityEndedHandle);
	MyMemory->AbilityEndedHandle.Reset();

	// Set scoot
	MyMemory->CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, nullptr);
}

void USBTTask_ExecuteSelectedAbility_Legacy::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FBTExecuteSelectedAbilityMemory_Legacy* MyMemory = CastInstanceNodeMemory< FBTExecuteSelectedAbilityMemory_Legacy>(CachedNodeMemory);
	if (MyMemory)
	{
		if (MyMemory->bAbilityEnded)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}

}

uint16 USBTTask_ExecuteSelectedAbility_Legacy::GetInstanceMemorySize() const
{
	return sizeof(FBTExecuteSelectedAbilityMemory_Legacy);
}
