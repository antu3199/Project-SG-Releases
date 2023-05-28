#pragma once
#include "CoreMinimal.h"
#include "SoulsGame/Animation/MyAnimNotifyState.h"
#include "SoulsGame/Animation/MyAnimNotify.h"

struct FSAnimationFileReaderStructNotifyAction
{
	float StartTime = 0;
	float Duration = 0;
	int32 TrackIndex = 0;
	FString RawEventName = "";
	TArray<FString> RawEventArgs;

	FSAnimationFileReaderStructNotifyAction(){}

	//FSAnimationFileReaderStructNotifyAction(float InStartTime, float InDuration, int32 InTrackIndex, const FString& InNotifyClass)
	//	: StartTime(InStartTime), Duration(InDuration), TrackIndex(InTrackIndex), NotifyClass(InNotifyClass)
	//{
	//}
};

struct FSAnimationFileReaderStructActions
{
	TArray<FSAnimationFileReaderStructNotifyAction> Actions;
};


class FSAssetTranslator
{
public:
	static bool TranslateMontageData(class USAssetComponent* InSAC);

	static FAnimNotifyEvent& CreateNewNotify(UAnimSequenceBase* Sequence, FString NewNotifyName, UClass* NotifyClass, int32 TrackIndex,  float StartTime, float NotifyStateDuration = 1 / 30.f);

private:
	static bool TranslateAnimationFileReaderStructToActions(const struct FSAnimationFileReaderStruct& FrameInfo, FSAnimationFileReaderStructActions & OutActions);

	static bool ApplyActionsToMontageData(class UAnimMontage* Montage, const FSAnimationFileReaderStructActions& Actions);

	static FVector GetVectorFromStr(const FString& Str);
};
