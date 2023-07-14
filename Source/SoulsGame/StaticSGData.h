#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SGAbilitySystem/SGAbility.h"
#include "Subsystems/SGRewardSubsystem.h"


#include "StaticSGData.generated.h"

#define SG_VERSION "0.1.2"

USTRUCT()
struct SOULSGAME_API FSavePointTransformData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FName PathName;


	bool Equals(const FSavePointTransformData Data)
	{
		return Location == Data.Location && Rotation == Data.Rotation && PathName == Data.PathName;
	}
};

USTRUCT(BlueprintType)
struct SOULSGAME_API FSavePointData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere)
	FString DisplayName;

	UPROPERTY()
	FSavePointTransformData TransformData;
};

UENUM()
enum class ESGStaticDataType
{
	Item,
	Relic
};

USTRUCT(BlueprintType)
struct FSGStaticInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESGStaticDataType StaticDataType;
};


USTRUCT(BlueprintType)
struct FSGStaticRelicItem : public FSGStaticInventoryItem
{
	GENERATED_BODY()

	FSGStaticRelicItem() : FSGStaticInventoryItem()
	{
		StaticDataType = ESGStaticDataType::Relic;
	}

	FGameplayTag GetAbilityTag() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<USGAbility> AbilityClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<USGEffect> AbilityEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class SOULSGAME_API USGStaticRelicData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSGStaticRelicItem> Relics;
};

UCLASS(BlueprintType, Blueprintable)
class SOULSGAME_API USGRewardData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSGRewardBlueprint> Rewards;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag OptionalRewardIdentifierTag;
};

UCLASS(BlueprintType, Blueprintable)
class SOULSGAME_API UStaticSGData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSavePointData> SavePointData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USGStaticRelicData* RelicData;

	UFUNCTION(BlueprintCallable)
	FString GetSGVersion() const;
	
	static FString GetVersion();
};
