#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGCharacterInventoryComponent.generated.h"

UCLASS()
class SOULSGAME_API USGCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USGCharacterInventoryComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int32 MaxHealthPots = 3;

	UFUNCTION(BlueprintCallable)
	void SetCurHealthPots(int32 Pots);

	UFUNCTION(BlueprintCallable)
	int32 GetCurHealthPots()
	{
		return CurHealthPots;
	}

	UFUNCTION(BlueprintCallable)
	void ResetAbilityUses();

	UPROPERTY(EditAnywhere)
	int32 InitialAbilityOneUses = 20;

	UPROPERTY(EditAnywhere)
	int32 InitialAbilityTwoUses = 5;

	// TODO change after ability system refactor
	UPROPERTY(BlueprintReadWrite)
	int32 TEMP_AbilityOneUses = -1;

	UPROPERTY(BlueprintReadWrite)
	int32 TEMP_AbilityTwoUses = -1;
	
private:
	int32 CurHealthPots;

	class UHUDWidget * GetHUD() const;
};
