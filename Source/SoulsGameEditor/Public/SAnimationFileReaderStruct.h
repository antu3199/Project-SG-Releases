#pragma once
#include "CoreMinimal.h"
#include "SAnimationFileReaderStruct.generated.h"


USTRUCT()
struct FSAnimationFileReaderAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString EventName;

	UPROPERTY()
	TArray<FString> EventArgs;

	static bool AreEventsEqual(const FSAnimationFileReaderAnimNotify& A, const FSAnimationFileReaderAnimNotify& B);
	bool IsEventEmpty() const;
	bool IsEqualTo(const FSAnimationFileReaderAnimNotify& B) const;
};


USTRUCT()
struct FSAnimationFileReaderEvent
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
	TArray<FSAnimationFileReaderAnimNotify> AnimNotifies;
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
	TArray<FSAnimationFileReaderEvent> Events;
};
