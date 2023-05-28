#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingTextActor.generated.h"

UCLASS(Abstract)
class SOULSGAME_API AFloatingTextActor : public AActor
{

	GENERATED_BODY()
public:
	AFloatingTextActor();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloatingTextActor")
	void Initialize(const FText& Text, const FColor& Color);

	const FVector& GetAnchorLocation() const { return AnchorLocation; }

	virtual void BeginDestroy() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	FVector AnchorLocation = FVector::ZeroVector;
};
