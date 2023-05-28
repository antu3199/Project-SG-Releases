#pragma once
#include "CoreMinimal.h"
#include "DetailCategoryBuilder.h"
#include "IDetailCustomization.h"
#include "SoulsGame/Tools/SAssetActor.h"

class FSAssetActorDetails : public IDetailCustomization
{
public:
	// Constructor.
	FSAssetActorDetails();

	// Destructor.
	virtual ~FSAssetActorDetails();

	// IDetailCustomization methods.
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	// Create an instance of this detail layout class.
	static TSharedRef<IDetailCustomization> MakeInstance();

	static void CreateWidget(
		IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
		const ASAssetActor* InAssetActor);

	
	TArray<ASAssetActor*> AssetActors;
};
