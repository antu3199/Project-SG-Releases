// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_ChooseAbility.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"
#include "SAIHelpers.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"

USBTTask_ChooseAbility::USBTTask_ChooseAbility()
{
	NodeName = "Choose Ability";
}

EBTNodeResult::Type USBTTask_ChooseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	if (!AbilitySystem)
	{
		return EBTNodeResult::Failed;
	}

	// if (AbilitySystem->IsUsingAnyAbility())
	// {
	// 	return EBTNodeResult::Failed;
	// }

	AActor * Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	PossibleAbilities = AbilitySystem->GetActivatableAbilities();
	if (PossibleAbilities.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	UAIAbilityHistory * AbilityHistory = Cast<UAIAbilityHistory>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHistoryBlackboardKey.SelectedKeyName));
	if (!AbilityHistory)
	{
		AbilityHistory = NewObject<UAIAbilityHistory>(this);
		AbilityHistory->Initialize();
	}

	TSet<FGameplayTag> NonCombatAbilities;
	// Maybe should just make this a tag later...
	NonCombatAbilities.Add(USGAbilitySystemGlobals::GetSG().Tag_Ability_Dash);
	NonCombatAbilities.Add(USGAbilitySystemGlobals::GetSG().Tag_Ability_Backstep);

	TArray<struct FGameplayAbilitySpec> CombatAbilities;
	CombatAbilities = PossibleAbilities.FilterByPredicate([=](const FGameplayAbilitySpec& Spec)
	{
		if (!Spec.Ability)
		{
			return false;
		}

		for (auto& AbilityTag : NonCombatAbilities)
		{
			if (Spec.Ability->AbilityTags.HasTag(AbilityTag))
			{
				return false;
			}
		}
	
		return true;
	});

	
	FGameplayAbilitySpec ExecutedAbilitySpec;

	bool bIsQueuedAction = false;

	if (AbilityHistory->HasLastQueuedAction())
	{
		ExecutedAbilitySpec = GetAbilitySpecFromContainer(AbilityHistory->GetLastQueuedAction());
		bIsQueuedAction = true;
	}
	else
	{
		bool bChooseRandomAbility = Debug_ChooseAbility == -1 || Debug_ChooseAbility >= PossibleAbilities.Num();

		if (bChooseRandomAbility)
		{
			if (CombatAbilities.Num() == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("No possible abilities"));
				return EBTNodeResult::Failed;
			}

			TArray<struct FGameplayAbilitySpec> FilteredCombatAbilities;
			if (AbilityHistory->AbilityHistoryItems.size() > 0 && CombatAbilities.Num() > 1)
			{
				// Don't use the same ability twice in a row
				FilteredCombatAbilities = CombatAbilities.FilterByPredicate([=](const FGameplayAbilitySpec& Spec)
				{
					if (!Spec.Ability)
					{
						return false;
					}

					if (Spec.Ability->AbilityTags.HasAll(AbilityHistory->AbilityHistoryItems.top().ExecutedAction))
					{
						return false;
					}
	
					return true;
				});
			}
			else
			{
				FilteredCombatAbilities = CombatAbilities;
			}

			if (FilteredCombatAbilities.Num() == 0)
			{
				return EBTNodeResult::Failed;
			}
		
			int32 RandInt = FMath::RandRange(0, FilteredCombatAbilities.Num() - 1);
			ExecutedAbilitySpec = FilteredCombatAbilities[RandInt];
		}
		else
		{
			ExecutedAbilitySpec = PossibleAbilities[Debug_ChooseAbility];
		}
	}

	FGameplayAbilitySpecHandle QueuedAbilityHandle;

	UMyGameplayAbility * ExecuteAbility = Cast<UMyGameplayAbility>(ExecutedAbilitySpec.Ability);
	UMyGameplayAbility* QueuedAbility = nullptr;

	if (!ExecuteAbility)
	{
		return EBTNodeResult::Failed;
	}

	UAIAbilityWrapper* AbilityWrapper = NewObject<UAIAbilityWrapper>(this);

	float DistanceFromTarget = FVector::Distance(Target->GetActorLocation(), Character->GetActorLocation());
	float MaxRangeToActivate = ExecuteAbility->GetMaxRangeToActivate(); // For melee
	float MinRangeToActivate = ExecuteAbility->GetMinRangeToActivate(); // For ranged

	AbilityWrapper->bDoWalk = false;

	FGameplayAbilitySpec DashSpec = GetAbilitySpecFromTag(USGAbilitySystemGlobals::GetSG().Tag_Ability_Dash);
	FGameplayAbilitySpec BackstepSpec = GetAbilitySpecFromTag(USGAbilitySystemGlobals::GetSG().Tag_Ability_Backstep);
	
	if (!bIsQueuedAction)
	{
		bool bIsMelee = ExecuteAbility->AbilityTags.HasTag(USGAbilitySystemGlobals::GetSG().Tag_AbilityType_Melee);


		if (bIsMelee && MaxRangeToActivate != -1.0f && DistanceFromTarget > MaxRangeToActivate)
		{
			bool bDoWalkBeforeDash = false;

			if (DashSpec.Handle.IsValid())
			{
				QueuedAbilityHandle = ExecutedAbilitySpec.Handle;
				ExecutedAbilitySpec = DashSpec;
				QueuedAbility = ExecuteAbility;
				ExecuteAbility = Cast<UMyGameplayAbility>(ExecutedAbilitySpec.Ability);
			}

			float RandChance =  FMath::RandRange(0, 1);
			//if (RandChance <= WalkBeforeDashChance)
			// TODO: Dash and walk
			{
				bDoWalkBeforeDash = true;

				RandChance = FMath::RandRange(0, 1);
				if (RandChance < MoveForwardsWhenUsingMeleeChance)
				{
					AbilityWrapper->WalkDirection = EAIWalkDirection::Forwards;
				}
				else
				{
					if (BackstepSpec.Handle.IsValid())
					{
						AbilityWrapper->WalkDirection = GetRandomWalkDirectionLeftRight();
					}
					else
					{
						AbilityWrapper->WalkDirection = GetRandomWalkDirectionLeftRightBack();
					}
				}
			}
			
			if (bDoWalkBeforeDash)
			{
				AbilityWrapper->bDoWalk = true;
			}
		}

		if (!bIsMelee && MinRangeToActivate != -1.0f && DistanceFromTarget < MinRangeToActivate)
		{
			if (BackstepSpec.Handle.IsValid())
			{
				QueuedAbilityHandle = ExecutedAbilitySpec.Handle;
				ExecutedAbilitySpec = BackstepSpec;
				QueuedAbility = ExecuteAbility;
				ExecuteAbility = Cast<UMyGameplayAbility>(ExecutedAbilitySpec.Ability);
			}
		}
	}

	// UE_LOG(LogTemp, Display, TEXT("Distance: %f Ability distance: %f"), DistanceFromTarget, AbilityDistance);
	AbilityWrapper->Initialize(ExecutedAbilitySpec.Handle);
	AbilityWrapper->MinimumActivationRange = MinRangeToActivate;
	AbilityWrapper->MaximumActivationRange = MaxRangeToActivate;

	if (QueuedAbility != nullptr)
	{
		AbilityWrapper->QueuedAbilitySpecHandle = QueuedAbilityHandle;
	}

	// To activate, do:
	// AbilitySystem->TryActivateAbility(AbilitySpec.Handle);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, AbilityWrapper);


	FAbilityHistoryItem HistoryItem;
	if (ExecuteAbility != nullptr)
	{
		HistoryItem.ExecutedAction = ExecuteAbility->AbilityTags;
		if (!bIsQueuedAction)
		{
			UE_LOG(LogTemp, Warning, TEXT("Executed action: %s"), *ExecuteAbility->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Executed queued action: %s"), *ExecuteAbility->GetName());
		}
	}
	if (QueuedAbility != nullptr)
	{
		HistoryItem.QueuedAction = QueuedAbility->AbilityTags;
	}


	if (QueuedAbility != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Queued action: %s"), *QueuedAbility->GetName());
	}

	AbilityHistory->AbilityHistoryItems.emplace(HistoryItem);

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHistoryBlackboardKey.SelectedKeyName, AbilityHistory);

	return EBTNodeResult::Succeeded;
	
}

FGameplayAbilitySpec USBTTask_ChooseAbility::GetAbilitySpecFromContainer(const FGameplayTagContainer& Container)
{
	FGameplayAbilitySpec* Spec = PossibleAbilities.FindByPredicate( [&](const FGameplayAbilitySpec& Spec)
	{
		return Spec.Ability != nullptr && Spec.Ability->AbilityTags.HasAll(Container);
	});

	if (Spec == nullptr)
	{
		return FGameplayAbilitySpec();
	}

	return *Spec;
}

FGameplayAbilitySpec USBTTask_ChooseAbility::GetAbilitySpecFromTag(const FGameplayTag& Tag)
{
	FGameplayTagContainer Container;
	Container.AddTag(Tag);

	return GetAbilitySpecFromContainer(Container);
}

EAIWalkDirection USBTTask_ChooseAbility::GetRandomWalkDirectionLeftRight() const
{
	float RandChance = FMath::RandRange(0, 1);
	if (RandChance <= 0.5f)
	{
		return EAIWalkDirection::Right;
	}
	else
	{
		return EAIWalkDirection::Left;
	}
}

EAIWalkDirection USBTTask_ChooseAbility::GetRandomWalkDirectionLeftRightBack() const
{
	float RandChance = FMath::RandRange(0, 1);
	if (RandChance <= 0.33f)
	{
		return EAIWalkDirection::Left;
	}
	else if (RandChance <= 0.66f)
	{
		return EAIWalkDirection::Right;
	}
	else
	{
		return EAIWalkDirection::Backwards;
	}
}
