#pragma once
#include "SoulsGame/SGAbilitySystem/SGAbility.h"
#include "SGAbility_DashUntilReachTarget.generated.h"

UCLASS()
class SOULSGAME_API USGAbility_DashUntilReachTarget : public USGAbility
{
	GENERATED_BODY()
	
public:
	void SetStoppingDistance(float Distance);

	virtual  void OnActivate_Implementation() override;
	
protected:
	virtual  void OnEndAbility_Implementation() override;
	
	bool UnFreezeCondition();

	void OnCharacterHitObject(class ASGCharacterBase* Character, AActor* Other, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float StoppingDistance = 150.0f;
	
	UPROPERTY(Transient)
	class UBlackboardComponent* BlackboardComponent = nullptr;

	UPROPERTY(Transient)
	class AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FName TargetActorBlackboardKey;

	bool bStopDueToHitActor = false;
	float CachedLastDistance = -1.0f;

};
