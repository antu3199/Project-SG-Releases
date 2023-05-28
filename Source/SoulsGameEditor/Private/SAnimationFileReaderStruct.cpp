#include "SoulsGameEditor/Public/SAnimationFileReaderStruct.h"

bool FSAnimationFileReaderAnimNotify::AreEventsEqual(const FSAnimationFileReaderAnimNotify& A,
	const FSAnimationFileReaderAnimNotify& B)
{
	if (A.EventName != B.EventName)
	{
		return false;
	}

	if (A.EventArgs.Num() != B.EventArgs.Num() )
	{
		return false;
	}

	for (int32 i = 0; i < A.EventArgs.Num(); i++)
	{
		if (A.EventArgs[i] != B.EventArgs[i])
		{
			return false;
		}
	}

	return true;
}

bool FSAnimationFileReaderAnimNotify::IsEventEmpty() const
{
	return EventName == "";
}

bool FSAnimationFileReaderAnimNotify::IsEqualTo(const FSAnimationFileReaderAnimNotify& B) const
{
	return AreEventsEqual(*this, B);
}
