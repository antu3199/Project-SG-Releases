#pragma once
#include "GameplayModMagnitudeCalculation.h"

#include "GameplayModRandomRangeMagnitudeCalculation.generated.h"


UCLASS(BlueprintType)
class UGameplayModRandomRangeMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	int32 TesterApples = 1234;
};
