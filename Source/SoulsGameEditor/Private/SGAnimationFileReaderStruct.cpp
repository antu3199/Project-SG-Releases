#include "SoulsGameEditor/Public/SGAnimationFileReaderStruct.h"

bool FSGAnimationFileReaderAnimNotify::AreEventsEqual(const FSGAnimationFileReaderAnimNotify& A,
	const FSGAnimationFileReaderAnimNotify& B)
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

bool FSGAnimationFileReaderAnimNotify::IsEventEmpty() const
{
	return EventName == "";
}

bool FSGAnimationFileReaderAnimNotify::IsEqualTo(const FSGAnimationFileReaderAnimNotify& B) const
{
	return AreEventsEqual(*this, B);
}
