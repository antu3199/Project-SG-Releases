
#include "SGBTTask_MoveTo.h"

#include "AIController.h"
#include "Tasks/AITask_MoveTo.h"

USGBTTask_MoveTo* USGBTTask_MoveTo::CreateAIMoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor,
    float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
    bool bUsePathfinding, bool bLockAILogic, bool bUseContinuosGoalTracking,
    EAIOptionFlag::Type ProjectGoalOnNavigation)
{
    USGBTTask_MoveTo* MyTask = Controller ? UAITask::NewAITask<USGBTTask_MoveTo>(*Controller, EAITaskPriority::High) : nullptr;
    if (MyTask)
    {
        FAIMoveRequest MoveReq;
        if (GoalActor)
        {
            MoveReq.SetGoalActor(GoalActor);
        }
        else
        {
            MoveReq.SetGoalLocation(GoalLocation);
        }

        MoveReq.SetAcceptanceRadius(AcceptanceRadius);
        MoveReq.SetReachTestIncludesAgentRadius(FAISystem::PickAIOption(StopOnOverlap, MoveReq.IsReachTestIncludingAgentRadius()));
        MoveReq.SetAllowPartialPath(FAISystem::PickAIOption(AcceptPartialPath, MoveReq.IsUsingPartialPaths()));
        MoveReq.SetUsePathfinding(bUsePathfinding);
        MoveReq.SetProjectGoalLocation(FAISystem::PickAIOption(ProjectGoalOnNavigation, MoveReq.IsProjectingGoal()));
        if (Controller)
        {
            MoveReq.SetNavigationFilter(Controller->GetDefaultNavigationFilterClass());
        }

        MyTask->SetUp(Controller, MoveReq);
        MyTask->SetContinuousGoalTracking(bUseContinuosGoalTracking);

        if (bLockAILogic)
        {
            MyTask->RequestAILogicLocking();
        }
    }

    return MyTask;
}

USGBTTask_MoveTo::USGBTTask_MoveTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

// TODO: on request failed or finished with stuff
void USGBTTask_MoveTo::ActivateAndBind()
{
    OnMoveFinished.AddDynamic(this, &USGBTTask_MoveTo::OnMoveTaskCompleted);
    OnRequestFailed.AddDynamic(this, &USGBTTask_MoveTo::OnMoveTaskFailed);

    
    Super::Activate();
}


FMoveTaskCompletedSignature& USGBTTask_MoveTo::GetMoveTaskCompletedDelegate()
{
    return OnMoveFinished;
}

UGameplayTask::FGenericGameplayTaskDelegate& USGBTTask_MoveTo::GetOnRequestFailedDelegate()
{
    
    return OnRequestFailed;
}

void USGBTTask_MoveTo::OnMoveTaskCompleted(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
    MoveTaskResultDelegate.Broadcast(true, Result);
    
    OnMoveFinished.RemoveDynamic(this, &USGBTTask_MoveTo::OnMoveTaskCompleted);
    OnRequestFailed.RemoveDynamic(this, &USGBTTask_MoveTo::OnMoveTaskFailed);
    
}

void USGBTTask_MoveTo::OnMoveTaskFailed()
{
    MoveTaskResultDelegate.Broadcast(false, EPathFollowingResult::Type::Invalid);
    OnMoveFinished.RemoveDynamic(this, &USGBTTask_MoveTo::OnMoveTaskCompleted);
    OnRequestFailed.RemoveDynamic(this, &USGBTTask_MoveTo::OnMoveTaskFailed);
}
