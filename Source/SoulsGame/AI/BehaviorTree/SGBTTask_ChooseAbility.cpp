// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTTask_ChooseAbility.h"


#include "AIController.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SGHumanoidCharacter.h"
#include "SoulsGame/AI/SGAIHelpers.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"

USGBTTask_ChooseAbility::USGBTTask_ChooseAbility()
{
	NodeName = "Choose Ability";
}

EBTNodeResult::Type USGBTTask_ChooseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	USGAbilityComponent* AbilitySystem = Character->FindComponentByClass<USGAbilityComponent>();
	if (!AbilitySystem)
	{
		return EBTNodeResult::Failed;
	}

	AActor * Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	TArray<FSGAbilitySpec> PossibleAbilities;
	AbilitySystem->GetActivatableAbilities(PossibleAbilities);
	if (PossibleAbilities.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	TMap<TSubclassOf<USGAbility>, FSGAbilitySpec*> AbilityClassToSpec;
	for (FSGAbilitySpec& AbilitySpec : PossibleAbilities)
	{
		AbilityClassToSpec.Add(AbilitySpec.AbilityClass, &AbilitySpec);
	}

	USGAIAbilityHistory * AbilityHistory = Cast<USGAIAbilityHistory>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHistoryBlackboardKey.SelectedKeyName));
	if (!AbilityHistory)
	{
		AbilityHistory = NewObject<USGAIAbilityHistory>(this);
		AbilityHistory->Initialize();
	}

	static const FGameplayTag DashTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Shared.DashUntilReachEnemy"));
	static const FGameplayTag BackstepTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Shared.Backstep"));
	
	TSet<FGameplayTag> NonCombatAbilities;
	// Maybe should just make this a tag later...
	NonCombatAbilities.Add(DashTag);
	NonCombatAbilities.Add(BackstepTag);

	static const FGameplayTag MeleeTypeTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("AbilityType.Melee"));
	static const FGameplayTag RangedTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("AbilityType.Ranged"));
	static const FGameplayTag MovementTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("AbilityType.Movement"));
	
	TArray<FSGAbilitySpec> CombatAbilities;
	CombatAbilities = PossibleAbilities.FilterByPredicate([=](const FSGAbilitySpec& Spec)
	{
		if (USGAbility* Ability = Cast<USGAbility>(Spec.AbilityClass->GetDefaultObject()))
		{
			if (Ability->AbilityTypeTags.HasTag(MovementTag))
			{
				return false;
			}
		}

		return true;
	});

	FSGAbilitySpec* ExecutedAbilitySpec = nullptr;
	bool bIsQueuedAction = false;

	if (AbilityHistory->HasLastQueuedAction())
	{
		ExecutedAbilitySpec = GetAbilitySpecFromTag(AbilityHistory->GetLastQueuedAction(), PossibleAbilities);
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

			TArray<FSGAbilitySpec> FilteredCombatAbilities;
			if (AbilityHistory->AbilityHistoryItems.size() > 0 && CombatAbilities.Num() > 1)
			{
				// Don't use the same ability twice in a row
				FilteredCombatAbilities = CombatAbilities.FilterByPredicate([=](const FSGAbilitySpec& Spec)
				{
					if (USGAbility* Ability = Cast<USGAbility>(Spec.AbilityClass->GetDefaultObject()))
					{
						if (Ability->AbilityTypeTags.HasTag(AbilityHistory->AbilityHistoryItems.top().ExecutedAction))
						{
							return false;
						}

						return true;
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
			ExecutedAbilitySpec = AbilityClassToSpec[FilteredCombatAbilities[RandInt].AbilityClass];
		}
		else
		{
			ExecutedAbilitySpec = &PossibleAbilities[Debug_ChooseAbility];
		}
	}

	check(ExecutedAbilitySpec != nullptr);
	USGAbility* ExecuteAbilityCDO = Cast<USGAbility>(ExecutedAbilitySpec->AbilityClass->GetDefaultObject());
	USGAbility* QueuedAbilityCDO = nullptr;

	if (!ExecuteAbilityCDO)
	{
		return EBTNodeResult::Failed;
	}

	FGameplayTag QueuedAbilityTag = ExecuteAbilityCDO->GetIdentifierTag();


	USGAIAbilityWrapper* AbilityWrapper = NewObject<USGAIAbilityWrapper>(this);

	float DistanceFromTarget = FVector::Distance(Target->GetActorLocation(), Character->GetActorLocation());
	float MaxRangeToActivate = ExecuteAbilityCDO->GetMaxRangeToActivate(); // For melee
	float MinRangeToActivate = ExecuteAbilityCDO->GetMinRangeToActivate(); // For ranged

	AbilityWrapper->bDoWalk = false;

	FSGAbilitySpec* DashSpec = GetAbilitySpecFromTag(DashTag, PossibleAbilities);
	FSGAbilitySpec* BackstepSpec = GetAbilitySpecFromTag(BackstepTag, PossibleAbilities);
	
	if (!bIsQueuedAction)
	{
		bool bIsMelee = ExecuteAbilityCDO->AbilityTypeTags.HasTag(MeleeTypeTag);


		if (bIsMelee && MaxRangeToActivate != -1.0f && DistanceFromTarget > MaxRangeToActivate)
		{
			bool bDoWalkBeforeDash = false;

			if (DashSpec)
			{
				ExecutedAbilitySpec = DashSpec;
				QueuedAbilityCDO = ExecuteAbilityCDO;
				ExecuteAbilityCDO = Cast<USGAbility>(ExecutedAbilitySpec->AbilityClass->GetDefaultObject());
			}

			float RandChance =  FMath::RandRange(0, 1);
			//if (RandChance <= WalkBeforeDashChance)
			// TODO: Dash and walk
			{
				bDoWalkBeforeDash = true;

				RandChance = FMath::RandRange(0, 1);
				if (RandChance < MoveForwardsWhenUsingMeleeChance)
				{
					AbilityWrapper->WalkDirection = ESGAIWalkDirection::Forwards;
				}
				else
				{
					if (BackstepSpec)
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
			if (BackstepSpec)
			{
				ExecutedAbilitySpec = BackstepSpec;
				QueuedAbilityCDO = ExecuteAbilityCDO;
				ExecuteAbilityCDO = Cast<USGAbility>(ExecutedAbilitySpec->AbilityClass->GetDefaultObject());
				
			}
		}
	}

	// UE_LOG(LogTemp, Display, TEXT("Distance: %f Ability distance: %f"), DistanceFromTarget, AbilityDistance);
	AbilityWrapper->Initialize(ExecuteAbilityCDO->GetIdentifierTag());
	AbilityWrapper->MinimumActivationRange = MinRangeToActivate;
	AbilityWrapper->MaximumActivationRange = MaxRangeToActivate;

	if (QueuedAbilityCDO != nullptr)
	{
		AbilityWrapper->QueuedAbilityTag = QueuedAbilityCDO->GetIdentifierTag();
	}

	// To activate, do:
	// AbilitySystem->TryActivateAbility(AbilitySpec.Handle);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, AbilityWrapper);


	FSGAbilityHistoryItem HistoryItem;
	if (ExecuteAbilityCDO != nullptr)
	{
		HistoryItem.ExecutedAction = ExecuteAbilityCDO->GetIdentifierTag();
		if (!bIsQueuedAction)
		{
			UE_LOG(LogTemp, Warning, TEXT("Executed action: %s"), *ExecuteAbilityCDO->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Executed queued action: %s"), *ExecuteAbilityCDO->GetName());
		}
	}
	if (QueuedAbilityCDO != nullptr)
	{
		HistoryItem.QueuedAction = QueuedAbilityCDO->GetIdentifierTag();
	}


	if (QueuedAbilityCDO != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Queued action: %s"), *QueuedAbilityCDO->GetName());
	}

	AbilityHistory->AbilityHistoryItems.emplace(HistoryItem);

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHistoryBlackboardKey.SelectedKeyName, AbilityHistory);

	return EBTNodeResult::Succeeded;
	
}

/*
FSGAbilitySpec* USBTTask_ChooseAbility::GetAbilitySpecFromContainer(const FGameplayTagContainer& Container, TArray<struct FSGAbilitySpec>& PossibleAbilities) const
{
	FSGAbilitySpec* Result = PossibleAbilities.FindByPredicate( [&](const FSGAbilitySpec& Spec)
	{
		if (const USGAbility* Ability = Cast<USGAbility>(Spec.AbilityClass->GetDefaultObject()))
		{
			return Spec.AbilityClass != nullptr && Ability->AbilityTypeTags.HasAll(Container);
		}
		
		return false;
	});

	return Result;
}
*/

FSGAbilitySpec* USGBTTask_ChooseAbility::GetAbilitySpecFromTag(const FGameplayTag& Tag, TArray<struct FSGAbilitySpec>& PossibleAbilities) const
{
	FSGAbilitySpec* Result = PossibleAbilities.FindByPredicate( [&](const FSGAbilitySpec& Spec)
	{
		if (const USGAbility* Ability = Cast<USGAbility>(Spec.AbilityClass->GetDefaultObject()))
		{
			return Spec.AbilityClass != nullptr && Ability->GetIdentifierTag() == Tag;
		}
		
		return false;
	});

	return Result;
}

FSGAbilitySpec* USGBTTask_ChooseAbility::GetAbilitySpecFromTypeTag(const FGameplayTag& TypeTag,
	TArray<FSGAbilitySpec>& PossibleAbilities) const
{
	FSGAbilitySpec* Result = PossibleAbilities.FindByPredicate( [&](const FSGAbilitySpec& Spec)
	{
		if (const USGAbility* Ability = Cast<USGAbility>(Spec.AbilityClass->GetDefaultObject()))
		{
			return Spec.AbilityClass != nullptr && Ability->AbilityTypeTags.HasTag(TypeTag);
			
		}
		
		return false;
	});

	return Result;
}

ESGAIWalkDirection USGBTTask_ChooseAbility::GetRandomWalkDirectionLeftRight() const
{
	float RandChance = FMath::RandRange(0, 1);
	if (RandChance <= 0.5f)
	{
		return ESGAIWalkDirection::Right;
	}
	else
	{
		return ESGAIWalkDirection::Left;
	}
}

ESGAIWalkDirection USGBTTask_ChooseAbility::GetRandomWalkDirectionLeftRightBack() const
{
	float RandChance = FMath::RandRange(0, 1);
	if (RandChance <= 0.33f)
	{
		return ESGAIWalkDirection::Left;
	}
	else if (RandChance <= 0.66f)
	{
		return ESGAIWalkDirection::Right;
	}
	else
	{
		return ESGAIWalkDirection::Backwards;
	}
}
