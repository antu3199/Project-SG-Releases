
#include "SBTTask_MoveTo.h"

#include "AIController.h"
#include "Tasks/AITask_MoveTo.h"

USBTTask_MoveTo* USBTTask_MoveTo::CreateAIMoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor,
    float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
    bool bUsePathfinding, bool bLockAILogic, bool bUseContinuosGoalTracking,
    EAIOptionFlag::Type ProjectGoalOnNavigation)
{
    USBTTask_MoveTo* MyTask = Controller ? UAITask::NewAITask<USBTTask_MoveTo>(*Controller, EAITaskPriority::High) : nullptr;
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

USBTTask_MoveTo::USBTTask_MoveTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

// TODO: on request failed or finished with stuff
void USBTTask_MoveTo::ActivateAndBind()
{
    OnMoveFinished.AddDynamic(this, &USBTTask_MoveTo::OnMoveTaskCompleted);
    OnRequestFailed.AddDynamic(this, &USBTTask_MoveTo::OnMoveTaskFailed);

    
    Super::Activate();
}


FMoveTaskCompletedSignature& USBTTask_MoveTo::GetMoveTaskCompletedDelegate()
{
    return OnMoveFinished;
}

UGameplayTask::FGenericGameplayTaskDelegate& USBTTask_MoveTo::GetOnRequestFailedDelegate()
{
    
    return OnRequestFailed;
}

void USBTTask_MoveTo::OnMoveTaskCompleted(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
    MoveTaskResultDelegate.Broadcast(true, Result);
    
    OnMoveFinished.RemoveDynamic(this, &USBTTask_MoveTo::OnMoveTaskCompleted);
    OnRequestFailed.RemoveDynamic(this, &USBTTask_MoveTo::OnMoveTaskFailed);
    
}

void USBTTask_MoveTo::OnMoveTaskFailed()
{
    MoveTaskResultDelegate.Broadcast(false, EPathFollowingResult::Type::Invalid);
    OnMoveFinished.RemoveDynamic(this, &USBTTask_MoveTo::OnMoveTaskCompleted);
    OnRequestFailed.RemoveDynamic(this, &USBTTask_MoveTo::OnMoveTaskFailed);
}
