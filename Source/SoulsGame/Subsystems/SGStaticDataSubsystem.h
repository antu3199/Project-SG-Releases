#pragma once

#include "CoreMinimal.h"
#include "SoulsGame/StaticSGData.h"

#include "SGStaticDataSubsystem.generated.h"


UCLASS(config = Game, meta = (DisplayName = "SG StaticData Subsystem"), defaultconfig)
class USGUIStaticDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	
public:
	//UPROPERTY(Config, EditDefaultsOnly)
	//TSubclassOf<class USGHUDWidget> HUDClass;
};

UCLASS()
class USGStaticDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintCallable)
	UStaticSGData* GetStaticSGData() const;



	
private:
};
