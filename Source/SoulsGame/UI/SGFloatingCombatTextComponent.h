#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGFloatingCombatTextComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSGAME_API USGFloatingCombatTextComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USGFloatingCombatTextComponent();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingCombatTextComponent")
	void AddFloatingText_Client(const FText& Text, FVector WorldLocation, const FColor& Color);
	
private:

	// Callback for when a floating text actordestroyed
	UFUNCTION()
	void OnTextDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingCombatTextComponent", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASGFloatingTextActor> FloatingTextActorClass = nullptr;


	// Amount of vertical space between each damage number
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingCombatTextComponent", Meta = (AllowPrivateAccess = "true"))
	float TextVerticalSpacing = 40.0f;

	// Amount of vertical offset to apply when spawning a new text actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingCombatTextComponent", Meta = (AllowPrivateAccess = "true"))
	float TextVerticalOffset = 100.0f;

	// AN array of all active floating text actor
	UPROPERTY(Transient)
	TArray<class ASGFloatingTextActor*> ActiveTextActors;
};
