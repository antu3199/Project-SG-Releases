#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "SGBTTask_MoveToUsingAbility.generated.h"

class UBehaviorTree;

/**
 * Move Directly Toward task node.
 * Moves the AI pawn toward the specified Actor or Location (Vector) blackboard entry in a straight line, without regard to any navigation system. If you need the AI to navigate, use the "Move To" node instead.
 */
UCLASS()
class SOULSGAME_API USGBTTask_MoveToUsingAbility : public UBTTask_MoveTo
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;

	/** fixed distance added to threshold between AI and goal location in destination reach test */
	UPROPERTY(Category = Node, EditAnywhere)
	float DefaultAcceptableRadius = 150.0f;
	
	virtual UAITask_MoveTo* PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest) override;
};

