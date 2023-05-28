#pragma once

#include "Blueprint/UserWidget.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Subsystems/SGTableRowData.h"
#include "SGNarrativeWidget.generated.h"


USTRUCT(BlueprintType)
struct SOULSGAME_API FSGRuntimeNarrativeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TextIndex;
};


UCLASS(Abstract)
class SOULSGAME_API USGNarrativeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USGNarrativeWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void PlayNarrativeText(const FSGNarrativeTableData& Data);

	UFUNCTION(BlueprintCallable)
	void ResetRuntimeData();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayNarrativeText_BP(const FText& Data, float DisplayForSeconds);

private:
	UFUNCTION(BlueprintCallable)
	float GetNarrationTime(int32 TextIndex);

	void EndNarration();

	void AdvanceNarrativeText();

	
	FSGRuntimeNarrativeData RuntimeData;
	FTimerHandle TimerHandle;
};


