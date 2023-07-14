#pragma once

#include "CoreMinimal.h"
#include "SGAIController.h"

#include "SGEnemyAIController.generated.h"

UCLASS(Blueprintable)
class ASGEnemyAIController : public ASGAIController
{
	GENERATED_BODY()

protected:
	virtual  void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(EditAnywhere)
	bool bUseDebugGameplayEffects = false;
	
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

