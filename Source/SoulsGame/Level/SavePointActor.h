#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InteractableActor.h"
#include "SoulsGame/SaveLoad/SGSaveRecord.h"

#include "SavePointActor.generated.h"

UCLASS(BlueprintType)
class USavePointEntryData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag Tag;
};


UCLASS(Abstract)
class SOULSGAME_API ASavePointActor : public AInteractableActor, public ISGSaveLoadInterface
{
	GENERATED_BODY()
public:
	ASavePointActor();

	virtual void SaveGame(struct FSGSaveRecord& SaveRecord) override;
	virtual void LoadGame(const struct FSGSaveRecord& LoadRecord) override;

	virtual  void BeginPlay() override;

	struct FSavePointTransformData GetSavepointTransformData() const;

	UFUNCTION(BlueprintCallable)
	void CheckUnlockVisibility();
	
	void SetVisibility(bool bIsVisible);
	bool IsVisible() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnlockVisibility();

	UFUNCTION(BlueprintCallable)
	void OnSavePointPressed();

	UFUNCTION(BlueprintCallable)
	FVector GetSpawnPoint() const;

	UFUNCTION(BlueprintCallable)
	FRotator GetSpawnRotation() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsUnlocked() const;

	UFUNCTION(BlueprintCallable)
	void SetUnlocked();

	UFUNCTION(BlueprintImplementableEvent)
	void OpenSaveUI(AMyPlayerController* Controller);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> PlayerSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SavePointTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer VisiblityRequirements;

protected:
	virtual void OnInteract(AMyPlayerController* Controller) override;

	bool bCurrentVisibility = false;


};
