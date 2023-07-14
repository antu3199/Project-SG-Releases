#pragma once
#include "CoreMinimal.h"
#include "SGAnimationFileReaderStruct.generated.h"


USTRUCT()
struct FSGAnimationFileReaderAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString EventName;

	UPROPERTY()
	TArray<FString> EventArgs;

	static bool AreEventsEqual(const FSGAnimationFileReaderAnimNotify& A, const FSGAnimationFileReaderAnimNotify& B);
	bool IsEventEmpty() const;
	bool IsEqualTo(const FSGAnimationFileReaderAnimNotify& B) const;
};


USTRUCT()
struct FSGAnimationFileReaderEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Frame;

	UPROPERTY()
	bool bHasTransformation;

	UPROPERTY()
	bool bHasData;
	
	UPROPERTY()
	TArray<FSGAnimationFileReaderAnimNotify> AnimNotifies;
};

USTRUCT()
struct FSAnimationFileReaderStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString AnimationName;

	UPROPERTY()
	int32 FrameStart;

	UPROPERTY()
	int32 FrameEnd;
	
	UPROPERTY()
	TArray<FSGAnimationFileReaderEvent> Events;
};
