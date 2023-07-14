#include "SoulsGameEditor/Public/SGEditorUtils.h"

#include "AssetThumbnail.h"
#include "DetailLayoutBuilder.h"
#include "Editor.h"
#include "PropertyCustomizationHelpers.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Widgets/Input/SComboButton.h"
#include "EditorWidgets/Public/SAssetDropTarget.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SoulsGameEditor/Public/SGAnimationFileReaderStruct.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "FSEditorUtils"

TSharedPtr<FAssetThumbnail> FSGEditorUtils::GetThumbnail(const UObject* ReferenceObject,
                                                        IDetailCategoryBuilder* InCategoryBuilder)
{
	TSharedPtr< FAssetThumbnailPool > AssetThumbnailPool = nullptr;
	if (InCategoryBuilder == nullptr)
	{
		AssetThumbnailPool = UThumbnailManager::Get().GetSharedThumbnailPool();
	}
	else
	{
		AssetThumbnailPool = InCategoryBuilder->GetParentLayout().GetThumbnailPool();
	}

	if (AssetThumbnailPool == nullptr)
	{
		return nullptr;
	}
	
	return MakeShareable(new FAssetThumbnail(ReferenceObject, 64, 64, AssetThumbnailPool));
}

void FSGEditorUtils::CreateAssetPicker(TSharedRef<SVerticalBox>& InVerticalBox,
	IDetailCategoryBuilder& InCategoryBuilder, UObject** ReferenceObject, const FString& ParameterLabel,
	const TFunction<bool(UObject*)>& OnSelectObject, const TArray<const UClass*>& AllowedClasses)
{
	if (ReferenceObject == nullptr)
	{
		return;
	}
	
	// Asset picker ==============================================
	TSharedPtr<FAssetThumbnail> AssetThumbnail = GetThumbnail(*ReferenceObject, &InCategoryBuilder);

	// Drop Target: Geometry collection
	TSharedPtr<SHorizontalBox> HorizontalBox = NULL;
	InVerticalBox->AddSlot().Padding( 0, 2 ).AutoHeight()
	[
		SNew( SAssetDropTarget )
		.OnAreAssetsAcceptableForDrop_Lambda([AllowedClasses](TArrayView<FAssetData> InAssets)
		{
			UObject* Asset = InAssets[0].GetAsset();
			return FSGEditorUtils::IsObjectAcceptable(Asset, AllowedClasses);
		})
		.OnAssetsDropped_Lambda([OnSelectObject](const FDragDropEvent&, TArrayView<FAssetData> InAssets)
		{
			UObject* InObject = InAssets[0].GetAsset();
			OnSelectObject(InObject);
		})
		[
			SAssignNew(HorizontalBox, SHorizontalBox)
		]
	];

	HorizontalBox->AddSlot()
	.Padding(5.0f, 0.0, 25.0f, 0.0)
	.VAlign(VAlign_Center)
	.AutoWidth()
	[
		SNew(STextBlock)
		.Text(FText::FromString(ParameterLabel))
	];
	
	// Thumbnail
	FText ParameterLabelText = FText::FromString(TEXT("No object selected"));
	if (*ReferenceObject != nullptr)
	{
		ParameterLabelText = FText::FromString((*ReferenceObject)->GetPathName());
	}
	
	TSharedPtr< SBorder > ThumbnailBorder;
	HorizontalBox->AddSlot().Padding(0.0f, 0.0f, 2.0f, 0.0f).AutoWidth()
	[
		SAssignNew(ThumbnailBorder, SBorder)
		.Padding(5.0f)
		.OnMouseDoubleClick_Lambda([ReferenceObject](const FGeometry&, const FPointerEvent&)
		{
			if (GEditor && *ReferenceObject)
				GEditor->EditObject(*ReferenceObject);

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
		[ReferenceObject, OnSelectObject, WeakAssetComboButton, AllowedClasses]()
		{
			UObject* DefaultObj = *ReferenceObject;

			TArray< UFactory * > NewAssetFactories;
			return PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
				FAssetData(DefaultObj),
				true,
				AllowedClasses,
				NewAssetFactories,
				FOnShouldFilterAsset(),
				FOnAssetSelected::CreateLambda(
					[OnSelectObject, WeakAssetComboButton](const FAssetData & AssetData)
					{
						TSharedPtr<SComboButton> ComboButton = WeakAssetComboButton.Pin();
						if (ComboButton.IsValid())
						{
							ComboButton->SetIsOpen(false);
							UObject * Object = AssetData.GetAsset();
							OnSelectObject(Object);
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
		PropertyCustomizationHelpers::MakeUseSelectedButton(FSimpleDelegate::CreateLambda([OnSelectObject, AllowedClasses]()
		{
		if (GEditor) 
		{
			TArray<FAssetData> CBSelections;
			GEditor->GetContentBrowserSelections(CBSelections);

			// Get the first selected asset object
			UObject* Object = nullptr;
			for (auto & CurAssetData : CBSelections) 
			{
				if (!HasClassName(CurAssetData.AssetClass, AllowedClasses))
				{
					continue;
				}

				Object = CurAssetData.GetAsset();
				break;
			}

			if (IsValid(Object)) 
			{
				OnSelectObject(Object);
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
			FSimpleDelegate::CreateLambda([ReferenceObject]()
			{
				if (GEditor && *ReferenceObject)
				{
					TArray<UObject*> Objects;
					Objects.Add(*ReferenceObject);
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
		.OnClicked_Lambda( [OnSelectObject]()
		{
			OnSelectObject(nullptr);
			return FReply::Handled();
		})
		[
			SNew( SImage )
			.Image( FEditorStyle::GetBrush( "PropertyWindow.DiffersFromDefault" ) )
		]
	];
}

void FSGEditorUtils::CreateTextBox(TSharedRef<SVerticalBox>& InVerticalBox, IDetailCategoryBuilder& InCategoryBuilder,
	FString* ReferenceStr, const FString& ParameterLabel, const TFunction<void(const FString& )>& OnTextChanged, const FString& ToolTip)
{
	TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);
	auto TextCommitted = [OnTextChanged](const FText& Val, ETextCommit::Type TextCommitType)
	{
		FString NewPathStr = Val.ToString();
		OnTextChanged(NewPathStr);
	};

	FText TooltipText = FText::Format(LOCTEXT("TooltipText",  "{0}"), FText::FromString(ToolTip));
	FText HintText = FText::Format(LOCTEXT("HintText",  "{0}"), FText::FromString(ToolTip));
	
	HorizontalBox->AddSlot()
	/*.AutoWidth()*/
	.Padding(5.0f, 0.0f, 6.0f, 0.0f)
	.MaxWidth(155.0f)
	[
		SNew(SBox)
		.IsEnabled_Lambda([]() { return true; })
		.WidthOverride(155.0f)
		[
			SNew(STextBlock)
			.Text(FText::Format(LOCTEXT("ParameterLabel",  "{0}"), FText::FromString(ParameterLabel)))
			.ToolTipText(TooltipText)
		]
	];
	
	HorizontalBox->AddSlot()
	/*.AutoWidth()*/
	.MaxWidth(235.0)
	[
		SNew(SBox)
		.IsEnabled_Lambda([]() { return true; })
		.WidthOverride(235.0f)
		[
			SNew(SEditableTextBox)
			.MinDesiredWidth(270)
			.ToolTipText(TooltipText)
			.HintText(HintText)
			.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			.Text_Lambda([ReferenceStr]()
			{
				if (ReferenceStr)
				{
					return FText::FromString(*ReferenceStr);
				}
				else
				{
					return FText();
				}
			})
			.OnTextCommitted_Lambda(TextCommitted)
		]
	];

	InVerticalBox->AddSlot().Padding( 0, 2 ).AutoHeight()
	[
		HorizontalBox
	];
}

void FSGEditorUtils::CreateButton(TSharedRef<SVerticalBox>& InVerticalBox, IDetailCategoryBuilder& InCategoryBuilder,
	const FString& ParameterLabel, const TFunction<FReply()>& OnButtonPress, const FString& ToolTip)
{

	TSharedRef<SHorizontalBox> ButtonRowHorizontalBox = SNew(SHorizontalBox);
	
	TSharedRef<SHorizontalBox> StartButtonHorizontalBox = SNew(SHorizontalBox);
	ButtonRowHorizontalBox->AddSlot()
	//.Padding(4.0, 0.0f, 0.0f, 0.0f)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.MaxWidth(135.0f)
	[
		SNew(SBox)
		.WidthOverride(135.0f)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.ToolTipText(FText::FromString(ToolTip))
			//.Text(FText::FromString("Asset Help"))
			.Visibility(EVisibility::Visible)
			.OnClicked_Lambda(OnButtonPress)
			.Content()
			[
				SAssignNew(StartButtonHorizontalBox, SHorizontalBox)
		]
		]
	];

	StartButtonHorizontalBox->AddSlot()
	.Padding(0.0, 0.0, 0.0, 0.0)
	.VAlign(VAlign_Center)
	.AutoWidth()
	[
		SNew(STextBlock)
		.Text(FText::FromString(ParameterLabel))
	];

	InVerticalBox->AddSlot().Padding( 0, 2 ).AutoHeight().HAlign(HAlign_Center)
	[
		ButtonRowHorizontalBox
	];
}

void FSGEditorUtils::ReadFromJson(const FString Path, FSAnimationFileReaderStruct& ResultStruct)
{
	FString JsonContentsStr;
	FFileHelper::LoadFileToString(JsonContentsStr, *Path);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonContentsStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FSGEditorUtils::GenerateStructsFromJson(ResultStruct, JsonObject);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize!"));
	}
}

void FSGEditorUtils::GenerateStructsFromJson(FSAnimationFileReaderStruct& ReaderObject,
                                            TSharedPtr<FJsonObject> JsonObject)
{
	ReaderObject.AnimationName = JsonObject->GetStringField("animation_name");
	ReaderObject.FrameStart = JsonObject->GetIntegerField("frame_start");
	ReaderObject.FrameEnd = JsonObject->GetIntegerField("frame_end");

	TArray<TSharedPtr<FJsonValue>> EventsArray = JsonObject->GetArrayField(TEXT("events"));
	for (int32 i = 0; i < EventsArray.Num(); i++)
	{
		TSharedPtr<FJsonValue> Value = EventsArray[i];
		TSharedPtr<FJsonObject> Json = Value->AsObject();
		FSGAnimationFileReaderEvent Event;
		GenerateStructsFromJson(Event, Json);
		ReaderObject.Events.Add(Event);
	}
}

void FSGEditorUtils::GenerateStructsFromJson(FSGAnimationFileReaderEvent& ReaderObject,
	TSharedPtr<FJsonObject> JsonObject)
{
	ReaderObject.Frame = JsonObject->GetIntegerField("frame");
	ReaderObject.bHasTransformation = JsonObject->GetBoolField("has_transformation");
	ReaderObject.bHasData = JsonObject->GetBoolField("has_data");

	TArray<TSharedPtr<FJsonValue>> AnimNotifiesArray = JsonObject->GetArrayField(TEXT("anim_notifies"));
	for (int32 i = 0; i < AnimNotifiesArray.Num(); i++)
	{
		TSharedPtr<FJsonValue> Value = AnimNotifiesArray[i];
		TSharedPtr<FJsonObject> Json = Value->AsObject();
		FSGAnimationFileReaderAnimNotify AnimNotify;
		GenerateStructsFromJson(AnimNotify, Json);
		ReaderObject.AnimNotifies.Add(AnimNotify);
	}
}

void FSGEditorUtils::GenerateStructsFromJson(FSGAnimationFileReaderAnimNotify& ReaderObject,
	TSharedPtr<FJsonObject> JsonObject)
{
	ReaderObject.EventName = JsonObject->GetStringField("event_name");

	if (!ReaderObject.EventName.IsEmpty())
	{
		// Only read the arguments if event is valid
		//ReaderObject.EventArg = JsonObject->GetStringField("event_arg");
		bool bArgArrayResult = JsonObject->TryGetStringArrayField("event_args", ReaderObject.EventArgs);
		if (!bArgArrayResult)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot find array"));
		}
	}
}

UClass* FSGEditorUtils::FindClass(const FString& ClassName)
{
	check(*ClassName);

	UObject* ClassPackage = ANY_PACKAGE;

	if (UClass* Result = FindObject<UClass>(ClassPackage, *ClassName))
		return Result;

	if (UObjectRedirector* RenamedClassRedirector = FindObject<UObjectRedirector>(ClassPackage, *ClassName))
		return CastChecked<UClass>(RenamedClassRedirector->DestinationObject);

	return nullptr;

}

float FSGEditorUtils::FramesToTime(float Frames)
{
	return (1 / 30.0f) * Frames;
}

bool FSGEditorUtils::IsObjectAcceptable(UObject* Asset, const TArray<const UClass*>& AllowedClasses)
{
	if (Asset == nullptr) return false;

	for (auto & Class : AllowedClasses)
	{
		if (Asset->IsA(Class)) return true;
	}
	
	return false;
}

bool FSGEditorUtils::HasClassName(const FName& Name, const TArray<const UClass*>& AllowedClasses)
{
	for (auto & Class : AllowedClasses)
	{
		if (Name == Class->GetFName())
		{
			return true;
		}
	}

	return false;
}


