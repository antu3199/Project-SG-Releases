#pragma once
#include "Tasks/AITask_MoveTo.h"
#include "SGBTTask_MoveTo.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FMoveTaskResultDelegate, bool bOk, TEnumAsByte<EPathFollowingResult::Type> Result);

UCLASS()
class SOULSGAME_API USGBTTask_MoveTo : public UAITask_MoveTo
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (AdvancedDisplay = "AcceptanceRadius,StopOnOverlap,AcceptPartialPath,bUsePathfinding,bUseContinuosGoalTracking,ProjectGoalOnNavigation", DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "Move To Location or Actor"))
    static USGBTTask_MoveTo* CreateAIMoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor = nullptr,
        float AcceptanceRadius = -1.f, EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default, EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
        bool bUsePathfinding = true, bool bLockAILogic = true, bool bUseContinuosGoalTracking = false, EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Default);

    USGBTTask_MoveTo(const FObjectInitializer& ObjectInitializer);
    
    void ActivateAndBind();

    FMoveTaskResultDelegate MoveTaskResultDelegate;
    
    FMoveTaskCompletedSignature& GetMoveTaskCompletedDelegate();
    FGenericGameplayTaskDelegate& GetOnRequestFailedDelegate();
protected:
    void OnMoveTaskCompleted(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);
    void OnMoveTaskFailed();
};
