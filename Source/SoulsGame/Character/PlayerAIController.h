#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "PlayerAIController.generated.h"

UCLASS(Blueprintable)
class APlayerAIController : public AAIController
{
	GENERATED_BODY()
public:

	virtual  void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;

	UPROPERTY(EditAnywhere)
	bool bUseDebugGameplayEffects = false;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UMyGameplayEffect>> Debug_PassiveGameplayEffects;
	
	UPROPERTY(Transient)
	class ANavMeshBoundsVolume* BoundingVolume = nullptr;

	bool IsLocationInBoundingVolume(const FVector& Location);

	UFUNCTION(BlueprintCallable)
	void SetAggro(bool bSet)
	{
		bIsAggroed = bSet;
	}
	bool IsAggroed() const
	{
		return bIsAggroed;
	}

	void SetHasTarget(bool bSet)
	{
		bHasTarget = bSet;
	}

	bool HasTarget() const
	{
		return bHasTarget;
	}

	UFUNCTION(BlueprintCallable)
	AActor* GetAITarget() const;

	
private:
	void InitializeBoundingVolume();
	
	bool bIsAggroed = false;
	bool bHasTarget = false;
};

