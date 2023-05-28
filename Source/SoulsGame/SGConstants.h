#pragma once

enum class ESGDebugCollisionLevel : uint8
{
	Deactivated = 0,
	Normal = 1,
	All = 2,
	MAX = 3
};

class FSGConstants
{
public:
	static ECollisionChannel CollisionChannel_Player;
	static ECollisionChannel CollisionChannel_Enemy;
	static ECollisionChannel CollisionChannel_PlayerDynamic;
	static ECollisionChannel CollisionChannel_EnemyDynamic;

	static ESGDebugCollisionLevel GetDebugCollisionLevel();

	static inline const float Ability_Default_Ranged_MinimumRange = 1000.0f;
	static inline const float Ability_Default_Ranged_AcceptableRadius = -1.0f;

	
	static inline const float Ability_Default_Melee_MinimumRange = -1.0f;
	static inline const float Ability_Default_Melee_AcceptableRadius = 200.0f;
	
};
