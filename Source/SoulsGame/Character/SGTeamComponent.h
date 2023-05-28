#pragma once
#include "CoreMinimal.h"
#include "SoulsGame/SGTeam.h"

#include "SGTeamComponent.generated.h"

UCLASS()
class USGTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ESGTeam GetTeam() const;

	void SetTeam(ESGTeam NewTeam);

	ESGTeam GetOpposingTeam() const;
	
protected:

	UPROPERTY(EditAnywhere)
	ESGTeam Team;
};
