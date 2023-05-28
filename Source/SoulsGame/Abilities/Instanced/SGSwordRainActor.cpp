#include "SGSwordRainActor.h"

#include "NiagaraComponent.h"
#include "SoulsGame/SHitboxManager.h"
#include "SoulsGame/Abilities/SGAbility_SwordRain.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
using namespace std::placeholders;

ASGSwordRainActor::ASGSwordRainActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	this->CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	this->CapsuleComponent->SetupAttachment(GetRootComponent());
}

void ASGSwordRainActor::BeginPlay()
{
	Super::BeginPlay();

	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	if (HitboxManager)
	{
		HitboxId = HitboxManager->RegisterExistingComponent(CapsuleComponent);
		FSGCollisionParams Params;
		Params.ProfileType = ESGCollisionProfileType::Profile_OverlapAllDynamic;
		Params.OverlapBehaviour = ESGTeamOverlapBehaviour::OnlyEnemies;
		HitboxManager->SetCollisionProfile(HitboxId, Params);
		
		HitboxManager->AddOnOverlapTickCallback(HitboxId, std::bind(&ASGSwordRainActor::OnDynamicHitboxTickOverlap, this, _1, _2), HitboxOverlapInterval);
	}
}

void ASGSwordRainActor::Initialize(USGAbility_SwordRain* InAbility)
{
	Ability = MakeWeakObjectPtr(InAbility);
}

void ASGSwordRainActor::PlayEffect()
{
	BP_PlayEffect();
}

void ASGSwordRainActor::StopEffect()
{
	BP_StopEffect();
}

void ASGSwordRainActor::BP_PlayEffect_Implementation()
{
	this->GetNiagaraComponent()->Activate();
}

void ASGSwordRainActor::BP_StopEffect_Implementation()
{
	this->GetNiagaraComponent()->Deactivate();
}

void ASGSwordRainActor::DestroySelf()
{
	if (Ability.IsValid())
	{
		Ability->OnSwordRainActorDestroy(this);
	}
	
	this->Destroy();
}

void ASGSwordRainActor::SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container)
{
	GameplayEffectDataContainer = Container;
}

TWeakPtr<FGameplayEffectsWrapperContainer> ASGSwordRainActor::GetGameplayEffectsWrapperContainer()
{
	return GameplayEffectDataContainer;
}

void ASGSwordRainActor::OnDynamicHitboxTickOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor)
{
	if (!GameplayEffectDataContainer.IsValid())
	{
		return;
	}

	if (OtherActor == this)
	{
		return;
	}

	ASCharacterBase* Character = Cast<ASCharacterBase>(OtherActor);
	
	if (GameplayEffectDataContainer.Pin()->CanApplyEffectToActor(0, OtherActor))
	{
		GameplayEffectDataContainer.Pin()->AddTargetToEffect(0, OtherActor);
		GameplayEffectDataContainer.Pin()->ApplyEffect(0);
	}

	GameplayEffectDataContainer.Pin()->ResetTargets();
}
