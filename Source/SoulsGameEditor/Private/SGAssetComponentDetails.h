#pragma once
#include "CoreMinimal.h"
#include "DetailCategoryBuilder.h"
#include "IDetailCustomization.h"
#include "SoulsGame/Tools/SGAssetComponent.h"
#include "SoulsGameEditor/Public/SGAnimationFileReaderStruct.h"

class FSGAssetComponentDetails : public IDetailCustomization
{
public:
	// Constructor.
	FSGAssetComponentDetails();

	// Destructor.
	virtual ~FSGAssetComponentDetails();

	// IDetailCustomization methods.
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	// Create an instance of this detail layout class.
	static TSharedRef<IDetailCustomization> MakeInstance();

	static void CreateWidget(
		IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
		USGAssetComponent* InSAC);

	static void PerformJsonOperation(USGAssetComponent* InSAC);
	
	static TSharedRef<class SVerticalBox> CreateSGContent(IDetailCategoryBuilder& InCategoryBuilder, USGAssetComponent * InSAC);
	static TSharedRef<class SVerticalBox> CreateExampleContent();
	static TArray<const UClass*> GetAllowedClasses();
	static bool IsObjectAcceptable(UObject* Asset);
	static bool HasClassName(const FName& Name);
	
	TArray<USGAssetComponent*> AssetComponents;
};
