#pragma once
#include "CoreMinimal.h"
#include "DetailCategoryBuilder.h"
#include "IDetailCustomization.h"
#include "SoulsGame/Tools/SAssetComponent.h"
#include "SoulsGameEditor/Public/SAnimationFileReaderStruct.h"

class FSAssetComponentDetails : public IDetailCustomization
{
public:
	// Constructor.
	FSAssetComponentDetails();

	// Destructor.
	virtual ~FSAssetComponentDetails();

	// IDetailCustomization methods.
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	// Create an instance of this detail layout class.
	static TSharedRef<IDetailCustomization> MakeInstance();

	static void CreateWidget(
		IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
		USAssetComponent* InSAC);

	static void PerformJsonOperation(USAssetComponent* InSAC);
	
	static TSharedRef<class SVerticalBox> CreateSGContent(IDetailCategoryBuilder& InCategoryBuilder, USAssetComponent * InSAC);
	static TSharedRef<class SVerticalBox> CreateExampleContent();
	static TArray<const UClass*> GetAllowedClasses();
	static bool IsObjectAcceptable(UObject* Asset);
	static bool HasClassName(const FName& Name);
	
	TArray<USAssetComponent*> AssetComponents;
};
