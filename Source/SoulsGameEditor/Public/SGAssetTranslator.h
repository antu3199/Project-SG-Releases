#pragma once
#include "CoreMinimal.h"
#include "SoulsGame/Animation/SGAnimNotifyState.h"
#include "SoulsGame/Animation/SGAnimNotify.h"

struct FSGAnimationFileReaderStructNotifyAction
{
	float StartTime = 0;
	float Duration = 0;
	int32 TrackIndex = 0;
	FString RawEventName = "";
	TArray<FString> RawEventArgs;

	FSGAnimationFileReaderStructNotifyAction(){}

	//FSAnimationFileReaderStructNotifyAction(float InStartTime, float InDuration, int32 InTrackIndex, const FString& InNotifyClass)
	//	: StartTime(InStartTime), Duration(InDuration), TrackIndex(InTrackIndex), NotifyClass(InNotifyClass)
	//{
	//}
};

struct FSAnimationFileReaderStructActions
{
	TArray<FSGAnimationFileReaderStructNotifyAction> Actions;
};


class FSGAssetTranslator
{
public:
	static bool TranslateMontageData(class USGAssetComponent* InSAC);

	static FAnimNotifyEvent& CreateNewNotify(UAnimSequenceBase* Sequence, FString NewNotifyName, UClass* NotifyClass, int32 TrackIndex,  float StartTime, float NotifyStateDuration = 1 / 30.f);

private:
	static bool TranslateAnimationFileReaderStructToActions(const struct FSAnimationFileReaderStruct& FrameInfo, FSAnimationFileReaderStructActions & OutActions);

	static bool ApplyActionsToMontageData(class UAnimMontage* Montage, const FSAnimationFileReaderStructActions& Actions);

	static FVector GetVectorFromStr(const FString& Str);
};
