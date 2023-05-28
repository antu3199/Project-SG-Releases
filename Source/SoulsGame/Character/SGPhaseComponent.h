#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SGPhaseComponent.generated.h"

USTRUCT(BlueprintType)
struct FSGPhase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float HealthPercent;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USGPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	virtual  void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	int32 GetPhase() const;

	void UpdatePhase();


	UPROPERTY(BlueprintAssignable)
	FOnPhaseChanged OnPhaseChanged;

protected:
	void OnTransitionPhase(int32 NewPhase);

	UAnimInstance* Helper_GetAnimMontage() const;
	
	// Phases, should be in decending order
	UPROPERTY(EditAnywhere)
	TArray<FSGPhase> Phases;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimMontage * StaggerMontage;

	
	int32 Phase = 0;
	FOnMontageEnded MontageEndedDelegate;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};

