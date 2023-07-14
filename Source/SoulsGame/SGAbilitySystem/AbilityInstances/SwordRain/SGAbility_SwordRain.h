#pragma once
#include "SoulsGame/SGAbilitySystem/SGAbility.h"
#include "SoulsGame/SGAbilitySystem/SGEffect.h"
#include "SGAbility_SwordRain.generated.h"

UCLASS()
class SOULSGAME_API USGAbility_SwordRain : public USGAbility
{

	
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;

	virtual void OnHandleEvent_Implementation(const FSGAbilityEventData& Payload) override;
	virtual void OnEndAbility_Implementation() override;
	
	void SpawnMarkers();
	

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
