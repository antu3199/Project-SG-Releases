#pragma once
#include "CoreMinimal.h"
#include "DetailCategoryBuilder.h"
#include "IDetailCustomization.h"
#include "SoulsGame/Tools/SGAssetActor.h"

class FSGAssetActorDetails : public IDetailCustomization
{
public:
	// Constructor.
	FSGAssetActorDetails();

	// Destructor.
	virtual ~FSGAssetActorDetails();

	// IDetailCustomization methods.
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	// Create an instance of this detail layout class.
	static TSharedRef<IDetailCustomization> MakeInstance();

	static void CreateWidget(
		IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
		const ASGAssetActor* InAssetActor);

	
	TArray<ASGAssetActor*> AssetActors;
};
