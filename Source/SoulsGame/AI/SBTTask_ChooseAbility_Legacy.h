// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_ChooseAbility_Legacy.generated.h"


UENUM(BlueprintType)
enum class ESGChooseAbilityType_Legacy : uint8
{
	ChooseAbilityType_Any UMETA(DisplayName="Any"),
	ChooseAbilityType_BasedOnDistance UMETA(DisplayName="BasedOnDistance"),
	ChooseAbilityType_MeleeOnly UMETA(DisplayName="MeleeOnly"),
	ChooseAbilityType_RangedOnly UMETA(DisplayName="RangedOnly")
};


/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTTask_ChooseAbility_Legacy : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USBTTask_ChooseAbility_Legacy();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityRangeBlackboardKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector TargetActorBlackboardKey;

	UPROPERTY(EditAnywhere)
	ESGChooseAbilityType_Legacy ChooseAbilityType;
	
	// Choose specific ability, for debugging purposes
	UPROPERTY(EditAnywhere)
	int32 Debug_ChooseAbility = -1;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
