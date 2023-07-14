// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulsGame/AI/SGAIHelpers.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SGBTTask_ChooseAbility.generated.h"


/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTTask_ChooseAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USGBTTask_ChooseAbility();

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
	//struct FSGAbilitySpec* GetAbilitySpecFromContainer(const FGameplayTagContainer& Container, TArray<struct FSGAbilitySpec>& PossibleAbilities) const;
	struct FSGAbilitySpec* GetAbilitySpecFromTag(const FGameplayTag& Tag, TArray<struct FSGAbilitySpec>& PossibleAbilities) const;

	struct FSGAbilitySpec* GetAbilitySpecFromTypeTag(const FGameplayTag& TypeTag, TArray<struct FSGAbilitySpec>& PossibleAbilities) const;

	
	ESGAIWalkDirection GetRandomWalkDirectionLeftRight() const;
	ESGAIWalkDirection GetRandomWalkDirectionLeftRightBack() const;

	
};
