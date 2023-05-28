#include "SHumanoidPlayerCharacter.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "SGCharacterInventoryComponent.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASHumanoidPlayerCharacter::ASHumanoidPlayerCharacter(const FObjectInitializer& ObjectInitializer)
    // Override character movement component
	: Super(ObjectInitializer)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // ConfectioniteActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ConfectioniteComponent"));
    // ConfectioniteActorComponent->SetupAttachment(this->GetMesh(), TEXT("spine3Socket"));
    // ConfectioniteActorComponent->SetEditorTreeViewVisualizationMode(EChildActorComponentTreeViewVisualizationMode::ComponentWithChildActor);

	CharacterInventoryComponent = CreateDefaultSubobject<USGCharacterInventoryComponent>(TEXT("CharacterInventoryComponent"));
}

void ASHumanoidPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CharacterMovementComponent->IsFalling())
	{
		bAbleToDash = true;
	}
}

void ASHumanoidPlayerCharacter::BeginShooting()
{
	if (bIsShooting)
	{
		return;
	}

	bIsShooting = true;

	UseAbility("Ability.Shoot");
	
}

void ASHumanoidPlayerCharacter::EndShooting()
{
	FGameplayTag Tag = USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Shoot_Stop;
	FGameplayEventData EventPayload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Tag, EventPayload);
	bIsShooting = false;
}

bool ASHumanoidPlayerCharacter::GetIsShooting() const
{
	return bIsShooting;
}

USGCharacterInventoryComponent* ASHumanoidPlayerCharacter::GetInventoryComponent() const
{
	return CharacterInventoryComponent;
}


// Called when the game starts or when spawned
void ASHumanoidPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsShooting = false;
}


