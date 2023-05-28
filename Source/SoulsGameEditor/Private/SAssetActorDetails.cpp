#include "SAssetActorDetails.h"

#include "DetailLayoutBuilder.h"
#include "SAssetComponentDetails.h"


#define LOCTEXT_NAMESPACE "SAssetActorDetails"

FSAssetActorDetails::FSAssetActorDetails()
{
}

FSAssetActorDetails::~FSAssetActorDetails()
{
}

void FSAssetActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
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
			ASAssetActor * AssetActor = Cast< ASAssetActor >(Object);
			USAssetComponent * SAC = AssetActor->GetAssetComponent();

			FString HoudiniEngineCategoryName = "SoulsGameTools";
			
			// Create Houdini Engine details category
			IDetailCategoryBuilder & HouEngineCategory =
				DetailBuilder.EditCategory(*HoudiniEngineCategoryName, FText::FromString(HoudiniEngineCategoryName), ECategoryPriority::Important);
		
			FSAssetComponentDetails::CreateWidget(HouEngineCategory, SAC);
			
			if (IsValid(SAC))
				AssetActors.Add(AssetActor);

			
		}
	}
}

TSharedRef<IDetailCustomization> FSAssetActorDetails::MakeInstance()
{
	return MakeShareable(new FSAssetActorDetails);
}

void FSAssetActorDetails::CreateWidget(IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
	const ASAssetActor* InAssetActor)
{
}
