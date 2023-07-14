#include "SGHumanoidPlayerCharacter.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "SGCharacterInventoryComponent.h"
#include "SGSlotComponent.h"
#include "SoulsGame/SGUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASGHumanoidPlayerCharacter::ASGHumanoidPlayerCharacter(const FObjectInitializer& ObjectInitializer)
    // Override character movement component
	: Super(ObjectInitializer)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // ConfectioniteActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ConfectioniteComponent"));
    // ConfectioniteActorComponent->SetupAttachment(this->GetMesh(), TEXT("spine3Socket"));
    // ConfectioniteActorComponent->SetEditorTreeViewVisualizationMode(EChildActorComponentTreeViewVisualizationMode::ComponentWithChildActor);

	CharacterInventoryComponent = CreateDefaultSubobject<USGCharacterInventoryComponent>(TEXT("CharacterInventoryComponent"));
	SlotComponent = CreateDefaultSubobject<USGSlotComponent>(TEXT("SlotComponent"));
}

void ASGHumanoidPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CharacterMovementComponent->IsFalling())
	{
		bAbleToDash = true;
	}
}

USGCharacterInventoryComponent* ASGHumanoidPlayerCharacter::GetInventoryComponent() const
{
	return CharacterInventoryComponent;
}


// Called when the game starts or when spawned
void ASGHumanoidPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}


