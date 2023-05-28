#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "SGUpgradeController.generated.h"

UENUM(BlueprintType)
enum class EUpgradeOptionType : uint8
{
	Unknown,
	DamageAdditive,
	DamageMultiplicative,
	UpgradeBasicShot,
	FireWhip
};

USTRUCT(BlueprintType)
struct FUpgradeOptionValue
{
	GENERATED_BODY()

	void SetTemplateValue(float RawValue);
	void SetTemplateValueMinMax(float ValueMin, float ValueMax);

	void CalculateValue();
	
	float GetValue() const;
	
	float ValueRandRangeMin = 0.0f;
	float ValueRandRangeMax = 0.0f;
	
	bool bHasTemplateValue = false;
	bool bHasCalculatedValue = false;
	bool bRandRange = false;

private:
	float Value = 0.0f;
	
};

USTRUCT(BlueprintType)
struct SOULSGAME_API FUpgradeOption
{
	GENERATED_BODY()

	FUpgradeOption() {}
	FUpgradeOption(const FString& InOptionName) : OptionName(InOptionName) {}

	FUpgradeOption(const EUpgradeOptionType& InOptionType)
	: OptionName(), UpgradeType(InOptionType) {}

	UPROPERTY(BlueprintReadWrite)
	FString OptionName = TEXT("Option 1");

	UPROPERTY(BlueprintReadWrite)
	EUpgradeOptionType UpgradeType = EUpgradeOptionType::Unknown;

	UPROPERTY(BlueprintReadWrite)
	FUpgradeOptionValue Value1;

	UPROPERTY(BlueprintReadWrite)
	FUpgradeOptionValue Value2;

	UPROPERTY(BlueprintReadWrite)
	FUpgradeOptionValue Value3;

	UPROPERTY(Transient)
	class UMyGameplayEffect* DynamicGameplayEffect = nullptr;

	void SetupOptionBeforeUse();
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpgradeApplied, FUpgradeOption UpgradeOption, AActor* Actor);

UCLASS(Blueprintable)
class SOULSGAME_API USGUpgradeController : public UObject
{
	GENERATED_BODY()
public:
	static USGUpgradeController* GetSingleton(const UObject* WorldContextObject);

	void InitializePool();

	bool GetNFromPool(const int32& N, TArray<FUpgradeOption>& OutUpgradeOptions, bool bAllowDuplicates = true) const;

	void ApplyUpgradeToCharacter(const FUpgradeOption& Option, class ASCharacterBase* Character);

	FOnUpgradeApplied OnUpgradeAppliedDelegate;

	class UMyGameplayAbility* GetGameplayAbilityFromGiveAbilityGETemplate(TSubclassOf<UMyGameplayEffect> Template) const;
	
	FGameplayTag GetGameplayTagFromGiveAbilityGETemplate(TSubclassOf<UMyGameplayEffect> Template) const;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMyGameplayEffect> GiveFireWhipEffectTemplate;
	
	TArray<FUpgradeOption> UpgradePool;

};
