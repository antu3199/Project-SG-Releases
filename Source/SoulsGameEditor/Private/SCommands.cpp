
#include "SoulsGameEditor/Public/SCommands.h"

#include "AssetThumbnail.h"
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
#include "SoulsGame/SGameSingleton.h"
#include "SoulsGame/Tools/SEditorInfoObject.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define LOCTEXT_NAMESPACE "FSCommands"

TSharedPtr<SWindow> FSCommands::SGWindow = nullptr;

FSCommands::FSCommands()
	: TCommands<FSCommands>	(TEXT("FSCommands"), NSLOCTEXT("Contexts", "FSCommands", "Souls Game Commands"), NAME_None, TEXT("StyleName"))
{
}

void
FSCommands::RegisterCommands()
{
	UI_COMMAND(_OpenSGWindow, "OpenSGWindow", "Opens SG Window", EUserInterfaceActionType::Button, FInputChord());
}

void FSCommands::OpenSGWindow()
{
	UE_LOG(LogTemp, Display, TEXT("OpenSGWindow"));


	// Create cook window
	SGWindow = SNew(SWindow)
	.Title(FText::FromString(TEXT("Cookbook Window")))
	.ClientSize(FVector2D(800, 400))
	.SupportsMaximize(false)
	.SupportsMinimize(false)
	;

	
	ForceUpdateWindow();
	OpenWindow(SGWindow);

	

	/*
		InVerticalBox->AddSlot().Padding(2, 2, 5, 2).AutoHeight()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(1.0f)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::Format(LOCTEXT("NumArrayItemsFmt", "{0} elements"), FText::AsNumber(NumInputObjects)))
			.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
		]
		+ SHorizontalBox::Slot()
		.Padding(1.0f)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateLambda([SetGeometryCollectionInputObjectsCount, InInputs, NumInputObjects]()
			{
				return SetGeometryCollectionInputObjectsCount(InInputs, NumInputObjects + 1);
			}),
			LOCTEXT("AddInput", "Adds a Geometry Input"), true)
		]
		+ SHorizontalBox::Slot()
		.Padding(1.0f)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateLambda([SetGeometryCollectionInputObjectsCount, InInputs]()
			{
				return SetGeometryCollectionInputObjectsCount(InInputs, 0);
			}),
			LOCTEXT("EmptyInputs", "Removes All Inputs"), true)
		]
	];
*/

	
}

TSharedRef< SVerticalBox > FSCommands::CreateSGContent()
{
	/*
	InVerticalBox->AddSlot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Hello from Slate")))
	];
	*/

	
	TSharedRef< SVerticalBox > InVerticalBox = SNew(SVerticalBox);
    	
	USEditorInfoObject* EditorObject = GetInfoObject();
	if (EditorObject == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Editor input object null"));
		return InVerticalBox;
	}
	
	TSharedPtr<FAssetThumbnailPool> AssetThumbnailPool = UThumbnailManager::Get().GetSharedThumbnailPool();
	// Get thumbnail pool for this builder.
	// TSharedPtr< FAssetThumbnailPool > AssetThumbnailPool = HouInputCategory.GetParentLayout().GetThumbnailPool();
	// 
	 // Create thumbnail for this static mesh.
	 TSharedPtr<FAssetThumbnail> AssetThumbnail = MakeShareable(
	 	new FAssetThumbnail(EditorObject->InputObject, 64, 64, AssetThumbnailPool));
	
	auto SelectObject = [EditorObject](UObject * InObject)
	{

		// TODO: May need to Modify/Save 
		if (InObject != nullptr)
		{
			//InObject->Modify();
		}
		
		EditorObject->SetInputObject(InObject);
		ForceUpdateWindow();
		
		return true;
	};

	// Drop Target: Geometry collection
	TSharedPtr<SHorizontalBox> HorizontalBox = NULL;
	InVerticalBox->AddSlot().Padding( 0, 2 ).AutoHeight()
	[
		SNew( SAssetDropTarget )
		.OnAreAssetsAcceptableForDrop_Lambda([](TArrayView<FAssetData> InAssets)
		{
			UObject* Asset = InAssets[0].GetAsset();
			bool bObjectAcceptable = IsObjectAcceptable(Asset);
			return IsObjectAcceptable(Asset);
		})
		.OnAssetsDropped_Lambda([SelectObject](const FDragDropEvent&, TArrayView<FAssetData> InAssets)
		{
			UObject* InObject = InAssets[0].GetAsset();
			SelectObject(InObject);
		})
		[
			SAssignNew(HorizontalBox, SHorizontalBox)
		]
	];

	// Thumbnail
	FText ParameterLabelText = FText::FromString(TEXT("Parameter"));

	if (EditorObject->InputObject != nullptr)
	{
		ParameterLabelText = FText::FromString(EditorObject->InputObject->GetPathName());
	}
	
	TSharedPtr< SBorder > ThumbnailBorder;
	HorizontalBox->AddSlot().Padding(0.0f, 0.0f, 2.0f, 0.0f).AutoWidth()
	[
		SAssignNew(ThumbnailBorder, SBorder)
		.Padding(5.0f)
		.OnMouseDoubleClick_Lambda([EditorObject](const FGeometry&, const FPointerEvent&)
		{
			if (GEditor && EditorObject->InputObject)
				GEditor->EditObject(EditorObject->InputObject);

			return FReply::Handled(); 
		})		
		[
			SNew(SBox)
			.WidthOverride(64)
			.HeightOverride(64)

			.ToolTipText(ParameterLabelText)
			[
			   AssetThumbnail->MakeThumbnailWidget()
			]
		]
	];

	FText InputObjectNameText = ParameterLabelText;

	TSharedPtr<SVerticalBox> ComboAndButtonBox;
	HorizontalBox->AddSlot()
	.FillWidth(1.0f)
	.Padding(0.0f, 4.0f, 4.0f, 4.0f)
	.VAlign(VAlign_Center)
	[
		SAssignNew(ComboAndButtonBox, SVerticalBox)
	];

	// Add Combo box
	// Combo button text
	TSharedPtr<SComboButton> AssetComboButton;
	ComboAndButtonBox->AddSlot().FillHeight(1.0f).VAlign(VAlign_Center)
	[
		SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1.0f).VAlign(VAlign_Center)
		[
			SAssignNew(AssetComboButton, SComboButton)
			.ButtonStyle(FEditorStyle::Get(), "PropertyEditor.AssetComboStyle")
			.ForegroundColor(FEditorStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
			.ContentPadding(2.0f)
			.ButtonContent()
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "PropertyEditor.AssetClass")
				.Font(FEditorStyle::GetFontStyle(FName(TEXT("PropertyWindow.NormalFont"))))
				.Text(InputObjectNameText)
			]
		]
	];

	// Combo button Asset picker
	TWeakPtr<SComboButton> WeakAssetComboButton(AssetComboButton);

	AssetComboButton->SetOnGetMenuContent(FOnGetContent::CreateLambda(
		[EditorObject, SelectObject, WeakAssetComboButton]()
		{
			TArray<const UClass *> AllowedClasses = GetAllowedClasses();
	
			UObject* DefaultObj = EditorObject->InputObject;

			TArray< UFactory * > NewAssetFactories;
			return PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
				FAssetData(DefaultObj),
				true,
				AllowedClasses,
				NewAssetFactories,
				FOnShouldFilterAsset(),
				FOnAssetSelected::CreateLambda(
					[SelectObject, WeakAssetComboButton](const FAssetData & AssetData)
					{
						TSharedPtr<SComboButton> ComboButton = WeakAssetComboButton.Pin();
						if (ComboButton.IsValid())
						{
							ComboButton->SetIsOpen(false);
							UObject * Object = AssetData.GetAsset();
							SelectObject(Object);
						}
					}
				),
				FSimpleDelegate::CreateLambda([]() {})
			);
		}
	));


	// Add buttons
	TSharedPtr<SHorizontalBox> ButtonHorizontalBox;
	ComboAndButtonBox->AddSlot()
	.FillHeight(1.0f)
	.Padding(0.0f, 4.0f, 4.0f, 4.0f)
	.VAlign(VAlign_Center)
	[
		SAssignNew(ButtonHorizontalBox, SHorizontalBox)
	];

	// Create tooltip.
	FFormatNamedArguments Args;
	Args.Add( TEXT( "Asset" ), InputObjectNameText );
	FText AssetTooltip = FText::Format(
		LOCTEXT("BrowseToSpecificAssetInContentBrowser",
			"Browse to '{Asset}' in Content Browser" ), Args );

	// Button : Use selected in content browser
	ButtonHorizontalBox->AddSlot()
	.AutoWidth()
	.Padding(2.0f, 0.0f)
	.VAlign(VAlign_Center)
	[
		PropertyCustomizationHelpers::MakeUseSelectedButton(FSimpleDelegate::CreateLambda([SelectObject]()
		{
		if (GEditor) 
		{
			TArray<FAssetData> CBSelections;
			GEditor->GetContentBrowserSelections(CBSelections);

			// Get the first selected asset object
			UObject* Object = nullptr;
			for (auto & CurAssetData : CBSelections) 
			{
				if (!HasClassName(CurAssetData.AssetClass))
				{
					continue;
				}

				Object = CurAssetData.GetAsset();
				break;
			}

			if (IsValid(Object)) 
			{
				SelectObject(Object);
			}
		}
		}), TAttribute< FText >(LOCTEXT("AssetInputUseSelectedAssetFromCB", "Use Selected Asset from Content Browser")))
	];

	// Button : Browse asset
	ButtonHorizontalBox->AddSlot()
	.AutoWidth()
	.Padding( 2.0f, 0.0f )
	.VAlign( VAlign_Center )
	[
		PropertyCustomizationHelpers::MakeBrowseButton(
			FSimpleDelegate::CreateLambda([EditorObject]()
			{
				if (GEditor && EditorObject->InputObject)
				{
					TArray<UObject*> Objects;
					Objects.Add(EditorObject->InputObject);
					GEditor->SyncBrowserToObjects(Objects);
				}
			}),
			TAttribute< FText >( AssetTooltip )
		)
	];

	// ButtonBox : Reset
	ButtonHorizontalBox->AddSlot()
	.AutoWidth()
	.Padding( 2.0f, 0.0f )
	.VAlign( VAlign_Center )
	[
		SNew( SButton )
		.ToolTipText( LOCTEXT( "ResetToBase", "Reset to default" ) )
		.ButtonStyle( FEditorStyle::Get(), "NoBorder" )
		.ContentPadding( 0 )
		.Visibility( EVisibility::Visible )
		.OnClicked_Lambda( [SelectObject]()
		{
			SelectObject(nullptr);
			return FReply::Handled();
		})
		[
			SNew( SImage )
			.Image( FEditorStyle::GetBrush( "PropertyWindow.DiffersFromDefault" ) )
		]
	];

	TSharedRef<SHorizontalBox> BakeFolderRowHorizontalBox = SNew(SHorizontalBox);


	auto OnBakeFolderTextCommittedLambda = [EditorObject](const FText& Val, ETextCommit::Type TextCommitType)
	{
		FString NewPathStr = Val.ToString();
		if (NewPathStr.IsEmpty())
			return;

		EditorObject->SetJsonObjectPath(NewPathStr);
	};
	
	BakeFolderRowHorizontalBox->AddSlot()
	/*.AutoWidth()*/
	.Padding(30.0f, 0.0f, 6.0f, 0.0f)
	.MaxWidth(155.0f)
	[
		SNew(SBox)
		.IsEnabled_Lambda([]() { return true; })
		.WidthOverride(155.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("JsonFileLabelLabel", "Json File"))
			.ToolTipText(LOCTEXT(
				"JsonFileLabelTooltip",
				"The Json file used to parse the information"
				))
		]
	];
	
	BakeFolderRowHorizontalBox->AddSlot()
	/*.AutoWidth()*/
	.MaxWidth(235.0)
	[
		SNew(SBox)
		.IsEnabled_Lambda([]() { return true; })
		.WidthOverride(235.0f)
		[
			SNew(SEditableTextBox)
			.MinDesiredWidth(270)
			.ToolTipText(LOCTEXT(
				"JsonFileLabelTooltip",
				"The Json file used to parse the information"))
			.HintText(LOCTEXT("sonFileLabelHintText", "Input to set json file"))
			.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			.Text_Lambda([EditorObject](){ return FText::FromString(EditorObject->JsonObjectPath); })
			.OnTextCommitted_Lambda(OnBakeFolderTextCommittedLambda)
		]
	];

	InVerticalBox->AddSlot().Padding( 0, 2 ).AutoHeight()
	[
		BakeFolderRowHorizontalBox
	];


	return InVerticalBox;
}

bool FSCommands::IsObjectAcceptable(UObject* Asset)
{
	if (Asset == nullptr) return false;
	TArray<const UClass*> AllowedClasses = GetAllowedClasses();

	for (auto & Class : AllowedClasses)
	{
		if (Asset->IsA(Class)) return true;
	}
	
	return false;
}

bool FSCommands::HasClassName(const FName& Name)
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

USGameSingleton* FSCommands::GetGameSingleton()
{
	USGameSingleton* GameSingleton = Cast<USGameSingleton>(GEngine->GameSingleton);
	return GameSingleton;
}

USEditorInfoObject* FSCommands::GetInfoObject()
{
	USGameSingleton* GameSingleton = GetGameSingleton();
	return GameSingleton->GetEditorObject();
}

void FSCommands::ForceUpdateWindow()
{
	TSharedRef< SVerticalBox > VerticalBox =  CreateSGContent(); 
	SGWindow->SetContent(VerticalBox);
}

void FSCommands::OpenWindow(TSharedPtr<SWindow> Window)
{
	if (Window == nullptr)
	{
		return;
	}
	
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));

	if (MainFrameModule.GetParentWindow().IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), MainFrameModule.GetParentWindow().ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}
}

TArray<const UClass*> FSCommands::GetAllowedClasses()
{
	TArray<const UClass*> AllowedClasses;
	AllowedClasses.Add(UAnimSequence::StaticClass());
	AllowedClasses.Add(UAnimMontage::StaticClass());

	//AllowedClasses.Add(UStaticMesh::StaticClass());
	//AllowedClasses.Add(USkeletalMesh::StaticClass());
	//AllowedClasses.Add(UBlueprint::StaticClass());
	return AllowedClasses;
}

