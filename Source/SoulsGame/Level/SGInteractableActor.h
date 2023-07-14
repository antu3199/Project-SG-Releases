#pragma once
#include "CoreMinimal.h"

#include "SGInteractableActor.generated.h"

UCLASS(BlueprintType)
class USGInteractEntryData : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	AActor* GetOwner() const { return Owner.IsValid() ? Owner.Get() : nullptr; }

	UPROPERTY(BlueprintReadOnly)
	FString DisplayText;
	
	TWeakObjectPtr<AActor> Owner;
};

UCLASS(Abstract)
class ASGInteractableActor : public AActor
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void QueryBeginInteract(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void QueryEndInteract(AActor* OtherActor);	
	
	UFUNCTION(BlueprintNativeEvent)
	void Interact(class ASGPlayerController* Controller);

	UFUNCTION(BlueprintNativeEvent)
	void UnInteract(class ASGPlayerController* Controller);
	
	UFUNCTION(BlueprintCallable)
	void Unlock();

	UFUNCTION(BlueprintCallable)
	void UnInteract_BP();
	
protected:
	virtual void OnInteract(class ASGPlayerController* Controller);
	
	UPROPERTY(EditAnywhere)
	bool bLockWhenInteract = false;

	UPROPERTY(EditAnywhere)
	FString InteractDisplayText = TEXT("Interact (F)");
	
	virtual void OnFKeyPressed();


	bool bInteracted = false;
	
	TWeakObjectPtr<class ASGCharacterBase> InteractedActor = nullptr;
};
