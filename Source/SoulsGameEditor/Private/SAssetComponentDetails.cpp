#include "SAssetComponentDetails.h"

#include "AssetThumbnail.h"
#include "DetailLayoutBuilder.h"
#include "Editor.h"
#include "EditorStyleSet.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/Commands.h"
#include "MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Text/STextBlock.h"

#include "PropertyCustomizationHelpers.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Editor/EditorEngine.h"
#include "EditorWidgets/Public/SAssetDropTarget.h"
#include "Widgets/Layout/SBorder.h"
#include "Framework/SlateDelegates.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SoulsGame/SGameSingleton.h"
#include "SoulsGame/Tools/SEditorInfoObject.h"
#include "SoulsGameEditor/Public/SAnimationFileReaderStruct.h"
#include "SoulsGameEditor/Public/SAssetTranslator.h"
#include "SoulsGameEditor/Public/SEditorUtils.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define LOCTEXT_NAMESPACE "SAssetComponentDetails"

FSAssetComponentDetails::FSAssetComponentDetails()
{
}

FSAssetComponentDetails::~FSAssetComponentDetails()
{
}

void FSAssetComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
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
			USAssetComponent * SAC = Cast< USAssetComponent >(Object);

			FString HoudiniEngineCategoryName = "SoulsGameTools";
			
			// Create Houdini Engine details category
			IDetailCategoryBuilder & HouEngineCategory =
				DetailBuilder.EditCategory(*HoudiniEngineCategoryName, FText::FromString(HoudiniEngineCategoryName), ECategoryPriority::Important);
		
			FSAssetComponentDetails::CreateWidget(HouEngineCategory, SAC);
			
			if (IsValid(SAC))
				AssetComponents.Add(SAC);

			
		}
	}
	
}

TSharedRef<IDetailCustomization> FSAssetComponentDetails::MakeInstance()
{
	return MakeShareable(new FSAssetComponentDetails);
}

void FSAssetComponentDetails::CreateWidget(IDetailCategoryBuilder& HoudiniEngineCategoryBuilder,
	USAssetComponent* InSAC)
{
	FDetailWidgetRow & Row = HoudiniEngineCategoryBuilder.AddCustomRow(FText::GetEmpty());

	TSharedRef< SVerticalBox > InVerticalBox = CreateSGContent(HoudiniEngineCategoryBuilder, InSAC);

	Row.WholeRowWidget.Widget = InVerticalBox;

}

void FSAssetComponentDetails::PerformJsonOperation(USAssetComponent* InSAC)
{
	bool bResult = FSAssetTranslator::TranslateMontageData(InSAC);
	if (bResult)
	{
		UE_LOG(LogTemp, Display, TEXT("Json operation successful!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Json operation unsuccessful!"));
	}
}


TSharedRef< SVerticalBox > FSAssetComponentDetails::CreateSGContent(IDetailCategoryBuilder& InCategoryBuilder, USAssetComponent * InSAC)
{

	// Get thumbnail pool for this builder.
	TSharedRef< SVerticalBox > InVerticalBox = SNew(SVerticalBox);
    	
	if (InSAC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Editor input object null"));
		return InVerticalBox;
	}

	FString ParameterLabel = TEXT("Asset");
	auto SelectObject = [&InCategoryBuilder, InSAC](UObject * InObject)
	{
		InSAC->SetInputObject(InObject);
		InSAC->Modify();
		
		if (InCategoryBuilder.IsParentLayoutValid())
			InCategoryBuilder.GetParentLayout().ForceRefreshDetails();
		
		return true;
	};
	
	FSEditorUtils::CreateAssetPicker(InVerticalBox, InCategoryBuilder, &InSAC->InputObject, ParameterLabel, SelectObject,  GetAllowedClasses());

	// Json Folder ====================================

	FString JsonParameterLabel = "Json file";

	auto OnJsonTextCommited = [InSAC, &InCategoryBuilder](const FString& Text)
	{
		if (Text.IsEmpty())
			return;

		InSAC->SetJsonObjectPath(Text);

		if (InCategoryBuilder.IsParentLayoutValid())
			InCategoryBuilder.GetParentLayout().ForceRefreshDetails();
	};

	FString JsonTooltip = TEXT("The Json file used to parse the information");
	
	FSEditorUtils::CreateTextBox(InVerticalBox, InCategoryBuilder, &InSAC->JsonObjectPath, JsonParameterLabel, OnJsonTextCommited, JsonTooltip);

	// Start button =========================================================

	auto OnStartButtonClickedLambda = [InSAC, &InCategoryBuilder]()
	{
		// TODO: Work with InSAC
		
		PerformJsonOperation(InSAC);
		
		if (InCategoryBuilder.IsParentLayoutValid())
			InCategoryBuilder.GetParentLayout().ForceRefreshDetails();
		
		return FReply::Handled();
	};

	FString StartButtonParameterName = TEXT("Start");
	FString StartButtonTooltip = "Start the operation!";
	FSEditorUtils::CreateButton(InVerticalBox, InCategoryBuilder, StartButtonParameterName, OnStartButtonClickedLambda, StartButtonTooltip);

	return InVerticalBox;
}

TSharedRef<SVerticalBox> FSAssetComponentDetails::CreateExampleContent()
{
	TSharedRef< SVerticalBox > InVerticalBox = SNew(SVerticalBox);
	
	InVerticalBox->AddSlot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Hello from Slate")))
	];

	return InVerticalBox;
}

TArray<const UClass*> FSAssetComponentDetails::GetAllowedClasses()
{
	TArray<const UClass*> AllowedClasses;
	//AllowedClasses.Add(UAnimSequence::StaticClass());
	AllowedClasses.Add(UAnimMontage::StaticClass());

	//AllowedClasses.Add(UStaticMesh::StaticClass());
	//AllowedClasses.Add(USkeletalMesh::StaticClass());
	//AllowedClasses.Add(UBlueprint::StaticClass());
	return AllowedClasses;
}

bool FSAssetComponentDetails::HasClassName(const FName& Name)
{
	TArray<const UClass*> AllowedClasses = GetAllowedClasses();

	for (auto & Class : AllowedClasses)
	{
		if (Name == Class->GetFName())
		{
			return true;
		}
	}

	return false;
}

bool FSAssetComponentDetails::IsObjectAcceptable(UObject* Asset)
{
	if (Asset == nullptr) return false;
	TArray<const UClass*> AllowedClasses = GetAllowedClasses();

	for (auto & Class : AllowedClasses)
	{
		if (Asset->IsA(Class)) return true;
	}
	
	return false;
}
