#include "SGAssetActorDetails.h"

#include "DetailLayoutBuilder.h"
#include "SGAssetComponentDetails.h"


#define LOCTEXT_NAMESPACE "SAssetActorDetails"

FSGAssetActorDetails::FSGAssetActorDetails()
{
}

FSGAssetActorDetails::~FSGAssetActorDetails()
{
}

void FSGAssetActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Get all components which are being customized.
	TArray< TWeakObjectPtr< UObject > > ObjectsCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsCustomized);
	
	// Extract the Houdini Asset Component to detail
	for (int32 i = 0; i < ObjectsCustomized.Num(); ++i)
	{
		if (ObjectsCustomized[i] == nullptr) continue;
		
		UObject * Object = ObjectsCustomized[i].Get();
		if (Object)
		{
			ASGAssetActor * AssetActor = Cast< ASGAssetActor >(Object);
			USGAssetComponent * SAC = AssetActor->GetAssetComponent();

			FString HoudiniEngineCategoryName = "SoulsGameTools";
			
			// Create Houdini Engine details category
			IDetailCategoryBuilder & HouEngineCategory =
				DetailBuilder.EditCategory(*HoudiniEngineCategoryName, FText::FromString(HoudiniEngineCategoryName), ECategoryPriority::Important);
		
			FSGAssetComponentDetails::CreateWidget(HouEngineCategory, SAC);
			
			if (IsValid(SAC))
				AssetActors.Add(AssetActor);

			
		}
	}
}

TSharedRef<IDetailCustomization> FSGAssetActorDetails::MakeInstance()
{
	return MakeShareable(new FSGAssetActorDetails);
}

void FSGAssetActorDetails::CreateWidget(IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
	const ASGAssetActor* InAssetActor)
{
}
