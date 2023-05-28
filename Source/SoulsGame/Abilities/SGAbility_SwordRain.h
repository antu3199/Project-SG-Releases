#pragma once
#include "PlayMontageCharacterAbility.h"
#include "Instanced/SGSwordRainActor.h"
#include "SoulsGame/Controllers/SGMarkerController.h"

#include "SGAbility_SwordRain.generated.h"

UCLASS()
class SOULSGAME_API USGAbility_SwordRain : public UPlayMontageCharacterAbility
{

	
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;

	virtual  void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

	class ASGSwordRainActor* CreateSwordRainActor(const FVector& ActorSpawnLocation);

	void OnSwordRainActorDestroy(class ASGSwordRainActor* Actor);

private:
	void OnCastTimeFinished(const FVector& CastLocation, const int32 MarkerId);

	void ActivateSwordRainAtLocation(const FVector& Location);
	
	UPROPERTY(EditAnywhere)
	float CastTime = 2.0f;

	UPROPERTY(EditAnywhere)
	FVector2f MarkerSize = FVector2f::One();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASGSwordRainActor> SwordRainActorTemplate;


	UPROPERTY(EditAnywhere)
	float SpawningOffset = 650.0f;
	
	UPROPERTY(Transient)
	TArray<class ASGSwordRainActor*> SwordRainActors;

	UPROPERTY(EditAnywhere)
	float TimeOffset = 0.2f;
	
	bool bActivated = false;

	int32 ExpectedNumberOfSwordRainActors;
	int32 FinishedSwordRainActors;

	TMap<int32, FTimerHandle> CastHandles;
};
