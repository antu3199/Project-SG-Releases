#pragma once

struct  FSGHandleGenerator
{
public:

	int32 GetNewHandle();
	void FreeHandle(int32 Handle);

	void Reset();

	bool IsActiveHandle(int32 Handle) const;
	
	static constexpr int32 Handle_Invalid = INDEX_NONE;
private:
	TSet<int32> m_ReusableHandles;
	
	int32 m_RunningHandleCount = 0;
};
