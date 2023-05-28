#pragma once
#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SoulsGame/SGCollisionProfile.h"
#include "SoulsGame/SGTeam.h"
#include "SoulsGame/Abilities/MyGameplayEffect.h"
#include "SGSplineComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USGSplineComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	USGSplineComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize(TWeakObjectPtr<class ASGSplineActor> BaseActor, TWeakObjectPtr<USplineComponent> Spline);

	UFUNCTION(BlueprintCallable)
	virtual void Start();

	UFUNCTION(BlueprintCallable)
	virtual void Stop();

	void OnComplete();

	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAlphaChangedBlueprint(float NewAlpha);
	
	virtual void OnAlphaChanged(float NewAlpha);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSplineLocationBlueprint(float Alpha, FVector Location, FRotator Rotation);
	virtual void OnSplineLocation(const float& Alpha, const FVector& Location, const FRotator& Rotation);

	float GetNormalizedLinearTime() const;
	float GetAlpha() const;

	float GetTotalDuration() const;
	
	void SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container);
	TWeakPtr<FGameplayEffectsWrapperContainer> GetGameplayEffectsWrapperContainer();

	void SetCollisionProfileType(FSGCollisionParams InCollisionParams);
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ESplineCoordinateSpace::Type> SplineCoordinateSpace = ESplineCoordinateSpace::Type::World;

	UPROPERTY(EditAnywhere)
	bool bAutoActivateSpline = true;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EEasingFunc::Type> EasingFunction;

	UPROPERTY(EditAnywhere)
	double EasingBlendExp = 2;

	UPROPERTY(EditAnywhere)
	int32 EasingSteps = 2;
	
	UPROPERTY(EditAnywhere)
	float Duration = 1.0f;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ParticleSystemTemplate = nullptr;

	UPROPERTY(Transient)
	class USceneComponent* SpawnedNiagaraSceneComponent = nullptr;
	
	UPROPERTY(Transient)
	class UNiagaraComponent* SpawnedNiagaraComponent = nullptr;

	UPROPERTY(EditAnywhere)
	float EndLoopDelay = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bLoop = true;

	UPROPERTY(EditAnywhere)
	bool bApplySplineRotation = true;
	
	UPROPERTY(EditAnywhere)
	bool bDeactivateNiagaraOnDetatch = true;
	
	UPROPERTY(EditAnywhere)
	FVector StartRotation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere)
	float StartDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Spline Hitbox")
	bool bSpawnHitbox = false;
	
	UPROPERTY(EditAnywhere, Category = "Spline Hitbox")
	FVector SplineHitboxExtents = FVector(1, 1, 1);


	UPROPERTY(EditAnywhere, Category = "Spline Hitbox")
	bool bDebugShowActiveHitbox = false;
	
	UPROPERTY(EditAnywhere, Category = "Spline Hitbox")
	FSGCollisionParams CollisionParams;

	
protected:
	
	void DetachExistingComponents();

	void ApplyGameplayEffectsToActor(AActor * Other);

	void RemoveGameplayEffects(UAbilitySystemComponent * AbilitySystemComponent);

	bool CanApplyEffectToActor(AActor* Other);

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	TWeakPtr<FGameplayEffectsWrapperContainer> GameplayEffectDataContainer = nullptr;;

	int32 ActiveHitboxId = -1;
	
	TWeakObjectPtr<ASGSplineActor> BaseActorRef = nullptr;
	TWeakObjectPtr<USplineComponent> Component = nullptr;
	
	float CurrentTime = 0.0f;
	bool bHasStarted = false;
	bool bHasStartedOnce = false;
	bool bHasInitialized = false;
	FTimerHandle StartTimerHandle;
	FTimerHandle LoopDelayHandle;
};
