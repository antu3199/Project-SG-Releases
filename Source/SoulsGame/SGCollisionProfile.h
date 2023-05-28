#pragma once

UENUM(BlueprintType)
enum class ESGCollisionProfileType : uint8
{
	Profile_NoCollision UMETA(DisplayName="NoCollision"),
	Profile_BlockAll UMETA(DisplayName="BlockAll"),
	Profile_OverlapAll UMETA(DisplayName="OverlapAll"),
	Profile_BlockAllDynamic UMETA(DisplayName="BlockAllDynamic"),
	Profile_OverlapAllDynamic UMETA(DisplayName="OverlapAllDynamic"),
	Profile_IgnoreOnlyPawn UMETA(DisplayName="IgnoreOnlyPawn"),
	Profile_OverlapOnlyPawn UMETA(DisplayName="OverlapOnlyPawn"),
	Profile_Pawn UMETA(DisplayName="Pawn"),
	Profile_Spectator UMETA(DisplayName="Spectator"),
	Profile_CharacterMesh UMETA(DisplayName="CharacterMesh"),
	Profile_PhysicsActor UMETA(DisplayName="PhysicsActor"),
	Profile_Destructible UMETA(DisplayName="Destructible"),
	Profile_InvisibleWall UMETA(DisplayName="InvisibleWall"),
	Profile_InvisibleWallDynamic UMETA(DisplayName="InvisibleWallDynamic"),
	Profile_Trigger UMETA(DisplayName="Trigger"),
	Profile_Ragdoll UMETA(DisplayName="Ragdoll"),
	Profile_Vehicle UMETA(DisplayName="Vehicle"),
	Profile_UI UMETA(DisplayName="UI"),
	// Game-specific
	Profile_PlayerPawn UMETA(DisplayName="PlayerPawn"),
	Profile_EnemyPawn UMETA(DisplayName="EnemyPawn"),
	Profile_OverlapAllExceptStatic UMETA(DisplayName="OverlapAllExceptStatic")
};

class SOULSGAME_API FSGCollisionProfileHelper
{
public:
	static FName GetCollisionProfileName(const ESGCollisionProfileType& ProfileType);
	static ESGCollisionProfileType GetCollisionProfileNameFromString(const FString& ProfileType);
};
