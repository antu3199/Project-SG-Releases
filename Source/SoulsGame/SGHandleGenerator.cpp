#include "SGHandleGenerator.h"

int32 FSGHandleGenerator::GetNewHandle()
{
	if (m_ReusableHandles.IsEmpty())
	{
		return m_RunningHandleCount++;
	}
	else
	{
		const int32 Id = *m_ReusableHandles.begin();
		m_ReusableHandles.Remove(Id);
		return Id;
	}
}

void FSGHandleGenerator::FreeHandle(int32 Handle)
{
	// Non-existing active handle?
	if (!IsActiveHandle(Handle))
	{
		return;
	}

	m_ReusableHandles.Add(Handle);
}

void FSGHandleGenerator::Reset()
{
	m_RunningHandleCount = 0;
	m_ReusableHandles.Reset();
}

bool FSGHandleGenerator::IsActiveHandle(int32 Handle) const
{
	return Handle < m_RunningHandleCount && !m_ReusableHandles.Contains(Handle);
}


