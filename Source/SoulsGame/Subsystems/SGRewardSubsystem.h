#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "SGRewardSubsystem.generated.h"


UCLASS(config = Game, meta = (DisplayName = "SG StaticData Subsystem"), defaultconfig)
class USGRewardSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	
public:
	//UPROPERTY(Config, EditDefaultsOnly)
	//TSubclassOf<class USGHUDWidget> HUDClass;
};

USTRUCT(BlueprintType)
struct FSGReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag RewardType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 1;
};

USTRUCT(BlueprintType)
struct FSGRewardBlueprint
{
	GENERATED_BODY()

	virtual ~FSGRewardBlueprint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Reward"))
	FGameplayTag RewardType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "QuantityMinCurve == nullptr"))
	int32 QuantityMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* QuantityMinCurve = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "QuantityMaxCurve == nullptr"))
	int32 QuantityMax = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* QuantityMaxCurve = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;
	
	virtual FSGReward CreateRewardInstance() const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardApplied, FSGReward, Reward);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardDataApplied, FGameplayTag, RewardTag);

UCLASS(BlueprintType)
class USGRewardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintCallable)
	void ApplyRewardBlueprint(struct FSGRewardBlueprint RewardBlueprint, class ASGCharacterBase* Character);
	
	UFUNCTION(BlueprintCallable)
	void ApplyReward(struct FSGReward Reward, class ASGCharacterBase* Character);

	void ApplyRewardData(TObjectPtr<class USGRewardData> RewardData, class ASGCharacterBase* Character);
	
	void ApplyRewardPtr(const FSGRewardBlueprint* RewardBlueprint, class ASGCharacterBase* Character);
	void ApplyRewardPtr(const FSGReward* Reward, class ASGCharacterBase* Character);

	UPROPERTY(BlueprintAssignable)
	FOnRewardApplied OnRewardApplied;
	
	UPROPERTY(BlueprintAssignable)
	FOnRewardDataApplied OnRewardDataApplied;
	
};
