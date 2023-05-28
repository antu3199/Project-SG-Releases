#include "SGCollisionProfile.h"

FName FSGCollisionProfileHelper::GetCollisionProfileName(const ESGCollisionProfileType& ProfileType)
{
	switch (ProfileType)
	{
	case ESGCollisionProfileType::Profile_NoCollision:
		return UCollisionProfile::NoCollision_ProfileName;
	case ESGCollisionProfileType::Profile_BlockAll:
		return UCollisionProfile::BlockAll_ProfileName;
	case ESGCollisionProfileType::Profile_OverlapAll:
		return FName("OverlapAll");
	case ESGCollisionProfileType::Profile_BlockAllDynamic:
		return FName("BlockAllDynamic");
	case ESGCollisionProfileType::Profile_OverlapAllDynamic:
		return FName("OverlapAllDynamic");
	case ESGCollisionProfileType::Profile_IgnoreOnlyPawn:
		return FName("IgnoreOnlyPawn");
	case ESGCollisionProfileType::Profile_OverlapOnlyPawn:
		return FName("OverlapOnlyPawn");
	case ESGCollisionProfileType::Profile_Pawn:
		return UCollisionProfile::Pawn_ProfileName;
	case ESGCollisionProfileType::Profile_Spectator:
		return FName("Spectator");
	case ESGCollisionProfileType::Profile_CharacterMesh:
		return FName("CharacterMesh");
	case ESGCollisionProfileType::Profile_PhysicsActor:
		return FName("PhysicsActor");
	case ESGCollisionProfileType::Profile_Destructible:
		return FName("Destructible");
	case ESGCollisionProfileType::Profile_InvisibleWall:
		return FName("InvisibleWall");
	case ESGCollisionProfileType::Profile_InvisibleWallDynamic:
		return FName("InvisibleWallDynamic");
	case ESGCollisionProfileType::Profile_Trigger:
		return FName("Trigger");
	case ESGCollisionProfileType::Profile_Ragdoll:
		return FName("Ragdoll");
	case ESGCollisionProfileType::Profile_Vehicle:
		return FName("Vehicle");
	case ESGCollisionProfileType::Profile_UI:
		return FName("UI");
	// Game-specific
	case ESGCollisionProfileType::Profile_PlayerPawn:
		return FName("PlayerPawn");
	case ESGCollisionProfileType::Profile_EnemyPawn:
		return FName("EnemyPawn");
	case ESGCollisionProfileType::Profile_OverlapAllExceptStatic:
		return FName("OverlapAllExceptStatic");
	default:
		check(false);
		return FName();
	}
}

ESGCollisionProfileType FSGCollisionProfileHelper::GetCollisionProfileNameFromString(const FString& ProfileType)
{
	if (ProfileType == "NoCollision")
	{
		return ESGCollisionProfileType::Profile_NoCollision;
	}
	else if (ProfileType == "BlockAll")
	{
		return ESGCollisionProfileType::Profile_NoCollision;
	}
	else if (ProfileType == "BlockAll")
	{
		return ESGCollisionProfileType::Profile_BlockAll;
	}
	else if (ProfileType == "OverlapAll")
	{
		return ESGCollisionProfileType::Profile_OverlapAll;
	}
	else if (ProfileType == "BlockAllDynamic")
	{
		return ESGCollisionProfileType::Profile_BlockAllDynamic;
	}
	else if (ProfileType == "OverlapAllDynamic")
	{
		return ESGCollisionProfileType::Profile_OverlapAllDynamic;
	}
	else if (ProfileType == "IgnoreOnlyPawn")
	{
		return ESGCollisionProfileType::Profile_IgnoreOnlyPawn;
	}
	else if (ProfileType == "OverlapOnlyPawn")
	{
		return ESGCollisionProfileType::Profile_OverlapOnlyPawn;
	}
	else if (ProfileType == "Pawn")
	{
		return ESGCollisionProfileType::Profile_Pawn;
	}
	else if (ProfileType == "Spectator")
	{
		return ESGCollisionProfileType::Profile_Spectator;
	}
	else if (ProfileType == "CharacterMesh")
	{
		return ESGCollisionProfileType::Profile_CharacterMesh;
	}
	else if (ProfileType == "PhysicsActor")
	{
		return ESGCollisionProfileType::Profile_PhysicsActor;
	}
	else if (ProfileType == "Destructible")
	{
		return ESGCollisionProfileType::Profile_Destructible;
	}
	else if (ProfileType == "InvisibleWall")
	{
		return ESGCollisionProfileType::Profile_InvisibleWall;
	}
	else if (ProfileType == "InvisibleWallDynamic")
	{
		return ESGCollisionProfileType::Profile_InvisibleWallDynamic;
	}
	else if (ProfileType == "Trigger")
	{
		return ESGCollisionProfileType::Profile_Trigger;
	}
	else if (ProfileType == "Ragdoll")
	{
		return ESGCollisionProfileType::Profile_Ragdoll;
	}
	else if (ProfileType == "Vehicle")
	{
		return ESGCollisionProfileType::Profile_Vehicle;
	}
	else if (ProfileType == "UI")
	{
		return ESGCollisionProfileType::Profile_UI;
	}
	else if (ProfileType == "PlayerPawn")
	{
		return ESGCollisionProfileType::Profile_PlayerPawn;
	}
	else if (ProfileType == "EnemyPawn")
	{
		return ESGCollisionProfileType::Profile_EnemyPawn;
	}
	else if (ProfileType == "OverlapAllExceptStatic")
	{
		return ESGCollisionProfileType::Profile_OverlapAllExceptStatic;
	}
	else
	{
		check(false);
		return ESGCollisionProfileType::Profile_NoCollision;
	}
}
