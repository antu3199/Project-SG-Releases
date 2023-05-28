#include "SGBTTask_MoveBeforeAbility.h"

#include "AIController.h"
#include "SAIHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Character/PlayerAIController.h"
#include "SoulsGame/Character/SCharacterBase.h"


USGBTTask_MoveBeforeAbility::USGBTTask_MoveBeforeAbility()
{
	NodeName = "Move To before ability";
	bNotifyTick = true;

	// TODO: Maybe it will be useful optimization to keep this off?
	//bCreateNodeInstance = true;
}

EBTNodeResult::Type USGBTTask_MoveBeforeAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FMoveBeforeAbilityMemory* MyMemory = reinterpret_cast<FMoveBeforeAbilityMemory*>(NodeMemory);
	MyMemory->CachedCharacter = MakeWeakObjectPtr(Cast<ASCharacterBase>(OwnerComp.GetAIOwner()->GetPawn()));
	if (!MyMemory->CachedCharacter.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	if (MyMemory->CachedCharacter->GetIsDead())
	{
		return EBTNodeResult::Failed;
	}

	UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(MyMemory->CachedCharacter->GetAbilitySystemComponent());
	if (!AbilitySystem)
	{
		return EBTNodeResult::Failed;
	}
	
	UAIAbilityWrapper * AbilityWrapper = Cast<UAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		return EBTNodeResult::Failed;
	}

	if (Debug_MoveDuration != 1.0f)
	{
		MyMemory->MoveDuration = Debug_MoveDuration;
	}

	MyMemory->bAbilityEnded = false;

	MyMemory->AcceptableDistance = AbilityWrapper->MaximumActivationRange;

	AActor * Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	MyMemory->CachedTarget = Target;

	MyMemory->CachedWalkDirection = AbilityWrapper->WalkDirection;
	MyMemory->CachedAIController = MakeWeakObjectPtr(OwnerComp.GetAIOwner());
	MyMemory->SecondsPassed = 0.0f;
	
	return EBTNodeResult::InProgress;
}

void USGBTTask_MoveBeforeAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FMoveBeforeAbilityMemory* MyMemory = CastInstanceNodeMemory<FMoveBeforeAbilityMemory>(NodeMemory);
	if (!MyMemory || !MyMemory->CachedCharacter.IsValid())
	{
		return;
	}

	if (MyMemory->bAbilityEnded)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	MyMemory->SecondsPassed += DeltaSeconds;
	if (MyMemory->SecondsPassed >= MyMemory->MoveDuration)
	{
		UAIAbilityWrapper * AbilityWrapper = Cast<UAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
		if (AbilityWrapper)
		{
			if (AbilityWrapper->WalkDirection != EAIWalkDirection::Forwards)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				return;
			}
		}
	}
	
	const float DistanceFromTarget = FVector::Distance(MyMemory->CachedCharacter->GetActorLocation(), MyMemory->CachedTarget->GetActorLocation());
	if (DistanceFromTarget <= MyMemory->AcceptableDistance)
	{
		SwapActionWithQueued(OwnerComp);
		EndTask(NodeMemory);
		return;
	}

	// If target has left bounding box, then stop

	if (APlayerAIController* CastedController = Cast<APlayerAIController>(MyMemory->CachedAIController))
	{
		const bool bInBoundingVolume = CastedController->IsLocationInBoundingVolume(MyMemory->CachedTarget->GetActorLocation());
		if (!bInBoundingVolume)
		{
			EndTask(NodeMemory);
		}
	}
	
	LookAtTarget(*MyMemory);

	FVector MoveDirection;

	if (MyMemory->CachedWalkDirection == EAIWalkDirection::Forwards)
	{
		MoveDirection = MyMemory->CachedCharacter->GetActorForwardVector();
	}
	else if (MyMemory->CachedWalkDirection == EAIWalkDirection::Backwards)
	{
		MoveDirection = -MyMemory->CachedCharacter->GetActorForwardVector();
	}
	else if (MyMemory->CachedWalkDirection == EAIWalkDirection::Right)
	{
		MoveDirection = MyMemory->CachedCharacter->GetActorRightVector();
	}
	else if (MyMemory->CachedWalkDirection == EAIWalkDirection::Left)
	{
		MoveDirection = -MyMemory->CachedCharacter->GetActorRightVector();
	}

	MyMemory->CachedCharacter->AddMovementInput(MoveDirection, MoveAxisScale);
}

void USGBTTask_MoveBeforeAbility::EndTask(uint8* NodeMemory)
{

	FMoveBeforeAbilityMemory* MyMemory = CastInstanceNodeMemory<FMoveBeforeAbilityMemory>(NodeMemory);
	if (!MyMemory)
	{
		return;
	}

	MyMemory->bAbilityEnded = true;
	MyMemory->SecondsPassed = 0.0f;
}



uint16 USGBTTask_MoveBeforeAbility::GetInstanceMemorySize() const
{
	return sizeof(FMoveBeforeAbilityMemory);
}

void USGBTTask_MoveBeforeAbility::SwapActionWithQueued(UBehaviorTreeComponent& OwnerComp)
{
	UAIAbilityWrapper * AbilityWrapper = Cast<UAIAbilityWrapper>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName));
	if (!AbilityWrapper)
	{
		return;
	}
	
	UAIAbilityHistory * AbilityHistory = Cast<UAIAbilityHistory>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityHistoryBlackboardKey.SelectedKeyName));
	if (!AbilityHistory)
	{
		return;
	}

	AbilityWrapper->ExecuteAbilitySpecHandle = AbilityWrapper->QueuedAbilitySpecHandle;
	AbilityWrapper->QueuedAbilitySpecHandle = FGameplayAbilitySpecHandle();

	FAbilityHistoryItem& HistoryItem = AbilityHistory->AbilityHistoryItems.top();
	HistoryItem.ExecutedAction = HistoryItem.QueuedAction;
	HistoryItem.QueuedAction.Reset();
	
	// Probably not needed, but do anyway
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHandleBlackboardKey.SelectedKeyName, AbilityWrapper);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AbilityHistoryBlackboardKey.SelectedKeyName, AbilityHistory);
}


void USGBTTask_MoveBeforeAbility::LookAtTarget(FMoveBeforeAbilityMemory& MyMemory)
{
	FSUtils::RotateCharacterToLookTarget(MyMemory.CachedCharacter.Get(), MyMemory.CachedTarget->GetActorLocation(), true);
}

