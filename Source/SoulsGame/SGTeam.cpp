#include "SGTeam.h"

#include "SUtils.h"
#include "Character/SGTeamComponent.h"

FSGOverlapContext::FSGOverlapContext()
{
}

FSGOverlapContext::FSGOverlapContext(AActor* InInstigator, AActor* InTarget, ESGTeamOverlapBehaviour InOverlapBehaviour)
{
	Instigator = MakeWeakObjectPtr(InInstigator);
	Target = MakeWeakObjectPtr(InTarget);
	OverlapBehaviour = InOverlapBehaviour;
}

bool FSGOverlapContext::ShouldOverlap() const
{
	if (OverlapBehaviour == ESGTeamOverlapBehaviour::DoNotOverlap)
	{
		return false;
	}
	
	if (!Instigator.IsValid() || !Target.IsValid())
	{
		return true; // Default is true
	}
	
	USGTeamComponent* InstigatorTeamComponent =  FSUtils::GetTeamComponent(Instigator.Get());
	USGTeamComponent* TargetTeamComponent =  FSUtils::GetTeamComponent(Target.Get());
	if (InstigatorTeamComponent == nullptr || TargetTeamComponent == nullptr)
	{
		return true;
	}

	if (OverlapBehaviour == ESGTeamOverlapBehaviour::OverlapAll)
	{
		return true;	
	}
	else if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyAllies)
	{
		return InstigatorTeamComponent->GetTeam() == TargetTeamComponent->GetTeam();
	}
	else if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyEnemies)
	{
		return InstigatorTeamComponent->GetTeam() != TargetTeamComponent->GetTeam();
	}
	else if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyAllies_ExceptSelf)
	{
		return InstigatorTeamComponent->GetTeam() == TargetTeamComponent->GetTeam() && Instigator != Target;
	}

	return true;
}
