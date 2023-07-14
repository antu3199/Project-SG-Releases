#include "SGLootActorEXP.h"

#include "SoulsGame/Character/SGCharacterBase.h"


void ASGLootActorEXP::ApplyLootToCharacter(ASGCharacterBase* Character)
{
	Super::ApplyLootToCharacter(Character);

	// DEPRECATED
	//Character->AddXp(1);
}

void ASGLootActorEXP::BeginPlay()
{
	Super::BeginPlay();
}

void ASGLootActorEXP::BeginDestroy()
{
	Super::BeginDestroy();
}
