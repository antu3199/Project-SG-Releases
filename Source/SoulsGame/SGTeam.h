#pragma once
#include "SGCollisionProfile.h"
#include "SGTeam.generated.h"

UENUM()
enum class ESGTeam : uint8
{
	Neutral,
	Player,
	Enemy
};

UENUM()
enum class ESGTeamOverlapBehaviour : uint8
{
	DoNotOverlap,
	OverlapAll,
	OnlyEnemies,
	OnlyAllies,
	OnlyAllies_ExceptSelf,
	OnlySelf
};

USTRUCT(BlueprintType)
struct FSGCollisionParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ESGCollisionProfileType ProfileType;

	// Only relevant if profile type is Overlap
	UPROPERTY(EditAnywhere)
	ESGTeamOverlapBehaviour OverlapBehaviour = ESGTeamOverlapBehaviour::OverlapAll;
};

struct FSGOverlapContext
{
	FSGOverlapContext();
	FSGOverlapContext(AActor* InInstigator, AActor* InTarget, ESGTeamOverlapBehaviour InOverlapBehaviour);

	bool ShouldOverlap() const;

	ESGTeamOverlapBehaviour OverlapBehaviour = ESGTeamOverlapBehaviour::OverlapAll;
	
	TWeakObjectPtr<AActor> Instigator = nullptr;
	TWeakObjectPtr<AActor> Target = nullptr;
};