#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "StaticSGData.generated.h"

#define SG_VERSION "0.1.1"

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

UCLASS(BlueprintType, Blueprintable)
class SOULSGAME_API UStaticSGData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FSavePointData> SavePointData;

	static FString GetVersion();
};
