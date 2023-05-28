#pragma once
#include "CoreMinimal.h"

#include "InteractableActor.generated.h"

UCLASS(BlueprintType)
class UInteractEntryData : public UObject
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
class AInteractableActor : public AActor
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void QueryBeginInteract(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void QueryEndInteract(AActor* OtherActor);	
	
	UFUNCTION(BlueprintNativeEvent)
	void Interact(class AMyPlayerController* Controller);

	UFUNCTION(BlueprintNativeEvent)
	void UnInteract(class AMyPlayerController* Controller);
	
	UFUNCTION(BlueprintCallable)
	void Unlock();

	UFUNCTION(BlueprintCallable)
	void UnInteract_BP();
	
protected:
	virtual void OnInteract(class AMyPlayerController* Controller);
	
	UPROPERTY(EditAnywhere)
	bool bLockWhenInteract = false;

	UPROPERTY(EditAnywhere)
	FString InteractDisplayText = TEXT("Interact (F)");
	
	virtual void OnFKeyPressed();


	bool bInteracted = false;
	
	TWeakObjectPtr<class ASCharacterBase> InteractedActor = nullptr;
};
