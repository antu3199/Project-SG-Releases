// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAIHelpers.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_ChooseAbility.generated.h"


/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTTask_ChooseAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USBTTask_ChooseAbility();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector TargetActorBlackboardKey;

	// Choose specific ability, for debugging purposes
	UPROPERTY(EditAnywhere)
	int32 Debug_ChooseAbility = -1;


	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHistoryBlackboardKey;

	UPROPERTY(EditAnywhere)
	float WalkBeforeDashChance = 0.6f;

	UPROPERTY(EditAnywhere)
	float MoveForwardsWhenUsingMeleeChance = 0.33f;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	struct FGameplayAbilitySpec GetAbilitySpecFromContainer(const FGameplayTagContainer& Container);
	struct FGameplayAbilitySpec GetAbilitySpecFromTag(const FGameplayTag& Tag);

	EAIWalkDirection GetRandomWalkDirectionLeftRight() const;
	EAIWalkDirection GetRandomWalkDirectionLeftRightBack() const;

	TArray<struct FGameplayAbilitySpec> PossibleAbilities;
	
};
