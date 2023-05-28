#pragma once
#include "CoreMinimal.h"
#include "NiagaraActor.h"
#include "Components/CapsuleComponent.h"
#include "SoulsGame/Abilities/MyGameplayAbility.h"

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


	void SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container);
	TWeakPtr<FGameplayEffectsWrapperContainer> GetGameplayEffectsWrapperContainer();

	
	void OnDynamicHitboxTickOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent * CapsuleComponent;

	UPROPERTY(EditAnywhere)
	float HitboxOverlapInterval = 1.0f;
	
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<USGAbility_SwordRain> Ability = nullptr;
	int32 HitboxId = -1;
	TWeakPtr<FGameplayEffectsWrapperContainer> GameplayEffectDataContainer = nullptr;
};
