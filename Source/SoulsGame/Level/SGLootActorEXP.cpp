#include "SGLootActorEXP.h"

#include "SoulsGame/Character/SCharacterBase.h"


void ASGLootActorEXP::ApplyLootToCharacter(ASCharacterBase* Character)
{
	Super::ApplyLootToCharacter(Character);

	Character->AddXp(1);
}

void ASGLootActorEXP::BeginPlay()
{
	Super::BeginPlay();
}

void ASGLootActorEXP::BeginDestroy()
{
	Super::BeginDestroy();
}
