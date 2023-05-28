
#include "SGConstants.h"
#include "algorithm"
static TAutoConsoleVariable<int32> CVarSGDebugCollisionLevel(
	TEXT("SoulsGame.DebugCollisionLevel"),
	1,
	TEXT("0: Deactivated, 1: Only important, 2: All"),
	ECVF_Default
	);



ECollisionChannel FSGConstants::CollisionChannel_Player = ECC_GameTraceChannel1;
ECollisionChannel FSGConstants::CollisionChannel_Enemy = ECC_GameTraceChannel2;
ECollisionChannel FSGConstants::CollisionChannel_PlayerDynamic = ECC_GameTraceChannel3;
ECollisionChannel FSGConstants::CollisionChannel_EnemyDynamic = ECC_GameTraceChannel4;

ESGDebugCollisionLevel FSGConstants::GetDebugCollisionLevel()
{
#if !WITH_EDITOR
	return static_cast<ESGDebugCollisionLevel>(0);
#endif
	
	int32 Value = CVarSGDebugCollisionLevel.GetValueOnAnyThread();

	if (Value >= static_cast<int32>(ESGDebugCollisionLevel::MAX))
	{
		Value = static_cast<int32>(ESGDebugCollisionLevel::MAX) - 1;
	}
	
	return static_cast<ESGDebugCollisionLevel>(Value);
}
