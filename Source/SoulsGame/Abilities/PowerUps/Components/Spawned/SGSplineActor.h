#pragma once
#include "CoreMinimal.h"
#include "SGSplineComponent.h"
#include "SoulsGame/Abilities/GameplayEffectActor.h"
#include "SoulsGame/Abilities/PowerUps/Components/SGPowerupComponent.h"
#include "SGSplineActor.generated.h"

UCLASS()
class ASGSplineActor : public AGameplayEffectActor
{
	GENERATED_BODY()
public:
	ASGSplineActor();

	virtual bool ShouldTickIfViewportsOnly() const;
	virtual void PostInitProperties() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void OnComponentFinished(USGSplineComponent* Component);

	void Start();
	void Stop();
	void SetDestroyOnStop();

	float GetTotalDuration() const;
	bool HasInitialized() const;
	void Initialize();

	virtual void SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container) override;

	FSGOnPowerupFinished OnPowerupFinishedDelegate;
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Base = nullptr;

	UPROPERTY(EditAnywhere)
	bool bTickInEditor = false;

	UPROPERTY(EditAnywhere)
	int32 NumSplines = 1;

	UPROPERTY(EditAnywhere)
	bool bLoop = true;
	
	UPROPERTY(EditAnywhere)
	bool bAutoActivateSpline = true;

	UPROPERTY(EditAnywhere)
	FSGCollisionParams CollisionParams;
	
	UPROPERTY()
	TArray<USplineComponent*> RealSplineComponents;

	UPROPERTY()
	TArray<USGSplineComponent*> SGComponents;

	UPROPERTY()
	TSet<USGSplineComponent*> FinishedComponents;

	bool bDestroyOnStop = false;
	bool bInitialized = false;
	
};
