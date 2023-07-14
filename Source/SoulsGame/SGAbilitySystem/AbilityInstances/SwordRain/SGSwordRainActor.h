#pragma once
#include "CoreMinimal.h"
#include "NiagaraActor.h"
#include "Components/CapsuleComponent.h"
#include "SoulsGame/SGAbilitySystem/SGEffect.h"
#include "SoulsGame/Subsystems/SGHitboxSubsystem.h"

#include "SGSwordRainActor.generated.h"

struct FGameplayEffectsWrapperContainer;

UCLASS(BlueprintType)
class ASGSwordRainActor : public ANiagaraActor
{
	GENERATED_BODY()
protected:
	ASGSwordRainActor(const FObjectInitializer& ObjectInitializer);

public:
	virtual  void BeginPlay() override;
	
	// Probably bad coding style, but whatever
	void Initialize(class USGAbility_SwordRain* InAbility);
	
	void PlayEffect();
	void StopEffect();

	UFUNCTION(BlueprintNativeEvent)
	void BP_PlayEffect();

	UFUNCTION(BlueprintNativeEvent)
	void BP_StopEffect();

	UFUNCTION(BlueprintCallable)
	void DestroySelf();

	virtual void SetEffectContext(const FSGEffectInstigatorContext& InEffectContext);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent * CapsuleComponent;

	UPROPERTY(EditAnywhere)
	float HitboxOverlapInterval = 1.0f;

	FSGEffectInstigatorContext EffectContext;
	
protected:
	UFUNCTION()
	void OnHitboxTriggered(class USGHitboxObject* HitboxObject, const FSGHitboxOverlapContext& OverlapContext);
	
	UPROPERTY(Transient)
	TWeakObjectPtr<USGAbility_SwordRain> Ability = nullptr;
	int32 HitboxId = -1;
};
