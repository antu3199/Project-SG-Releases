// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_ChooseAbility_Legacy.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"
#include "SAIHelpers.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"

USBTTask_ChooseAbility_Legacy::USBTTask_ChooseAbility_Legacy()
{
	NodeName = "Choose Ability";
}

EBTNodeResult::Type USBTTask_ChooseAbility_Legacy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (AbilitySystem->IsUsingAnyAbility())
	{
		return EBTNodeResult::Failed;
	}

	AActor * Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	float DistanceFromTarget = FVector::Distance(Target->GetActorLocation(), Character->GetActorLocation());
	
	TArray<FGameplayAbilitySpec> PossibleAbilities = AbilitySystem->GetActivatableAbilities();
	if (PossibleAbilities.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	bool bChooseRandomAbility = Debug_ChooseAbility == -1 || Debug_ChooseAbility >= PossibleAbilities.Num();
	FGameplayAbilitySpec AbilitySpec;

	if (bChooseRandomAbility)
	{
		switch (ChooseAbilityType)
		{
			case ESGChooseAbilityType_Legacy::ChooseAbilityType_BasedOnDistance:
				PossibleAbilities = PossibleAbilities.FilterByPredicate([DistanceFromTarget](auto& AbilitySpec)
				{
					if (UMyGameplayAbility * PossibleAbility = Cast<UMyGameplayAbility>(AbilitySpec.Ability))
					{
						if (PossibleAbility->GetMinimumRangeToActivate_Legacy() == -1.0f || DistanceFromTarget >= PossibleAbility->GetMinimumRangeToActivate_Legacy())
						{
							return true;
						}
					}
			
					return false;
				});

				break;
			case ESGChooseAbilityType_Legacy::ChooseAbilityType_MeleeOnly:
				PossibleAbilities = PossibleAbilities.FilterByPredicate([DistanceFromTarget](auto& AbilitySpec)
				{
					if (UMyGameplayAbility * PossibleAbility = Cast<UMyGameplayAbility>(AbilitySpec.Ability))
					{
						if (PossibleAbility->AbilityTags.HasTag(USGAbilitySystemGlobals::GetSG().Tag_AbilityType_Melee))
						{
							return true;
						}
					}
			
					return false;
				});
				break;
			case ESGChooseAbilityType_Legacy::ChooseAbilityType_RangedOnly:
				PossibleAbilities = PossibleAbilities.FilterByPredicate([DistanceFromTarget](auto& AbilitySpec)
				{
					if (UMyGameplayAbility * PossibleAbility = Cast<UMyGameplayAbility>(AbilitySpec.Ability))
					{
						if (PossibleAbility->AbilityTags.HasTag(USGAbilitySystemGlobals::GetSG().Tag_AbilityType_Ranged))
						{
							return true;
						}
					}
				
					return false;
				});
				break;
			default:
				break;
		}

		if (PossibleAbilities.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("No possible abilities"));
			return EBTNodeResult::Failed;
		}
		
		int32 RandInt = FMath::RandRange(0, PossibleAbilities.Num() - 1);
		AbilitySpec = PossibleAbilities[RandInt];
	}
	else
	{
		AbilitySpec = PossibleAbilities[Debug_ChooseAbility];
	}

	UMyGameplayAbility * Ability = Cast<UMyGameplayAbility>(AbilitySpec.Ability);

	if (!Ability)
	{
		return EBTNodeResult::Failed;
	}

	float AbilityDistance = Ability->GetMinimumRangeToActivate_Legacy();
	// UE_LOG(LogTemp, Display, TEXT("Distance: %f Ability distance: %f"), DistanceFromTarget, AbilityDistance);

	
	UAIAbilityWrapper_Legacy* AbilityWrapper = NewObject<UAIAbilityWrapper_Legacy>();
	AbilityWrapper->Initialize(AbilitySpec.Handle, Ability->GetMinimumRangeToActivate_Legacy(), Ability->GetMoveToAcceptableRadius_Legacy());


	// To activate, do:
	// AbilitySystem->TryActivateAbility(AbilitySpec.Handle);

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, AbilityWrapper);


	
	return EBTNodeResult::Succeeded;
	
}
