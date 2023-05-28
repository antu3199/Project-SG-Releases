#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "SGTableRowData.generated.h"

USTRUCT(BlueprintType)
struct SOULSGAME_API FLoreBitTableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FText Description;
};


USTRUCT(BlueprintType)
struct SOULSGAME_API FSGNarrativeTableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> Text;
};
