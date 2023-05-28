#pragma once

#include "CoreMinimal.h"
#include "AssetThumbnail.h"
#include "DetailCategoryBuilder.h"
#include "Components/VerticalBox.h"
#include "Dom/JsonObject.h"


struct FSAnimationFileReaderStruct;

class FSEditorUtils
{
public:
	static TSharedPtr<FAssetThumbnail> GetThumbnail(const UObject* ReferenceObject,IDetailCategoryBuilder* InCategoryBuilder = nullptr);
	static void CreateAssetPicker(TSharedRef< SVerticalBox >& InVerticalBox,
		IDetailCategoryBuilder& InCategoryBuilder, UObject** ReferenceObject,
		const FString& ParameterLabel,
		const TFunction<bool(UObject *)>& OnSelectObject,
		const TArray<const UClass*>& AllowedClasses
	);

	static void CreateTextBox(TSharedRef< SVerticalBox >& InVerticalBox,
		IDetailCategoryBuilder& InCategoryBuilder,
		FString* ReferenceStr,
		const FString& ParameterLabel,
		const TFunction<void(const FString& )>& OnTextChanged,
		const FString& ToolTip = TEXT("Tooltip!")
	);

	static void CreateButton(TSharedRef< SVerticalBox >& InVerticalBox,
		IDetailCategoryBuilder& InCategoryBuilder,
		const FString& ParameterLabel,
		const TFunction<FReply()>& OnButtonPress,
		const FString& ToolTip = TEXT("Tooltip!")
	);

	static void ReadFromJson(const FString Path, FSAnimationFileReaderStruct& ResultStruct);
	static void GenerateStructsFromJson(struct FSAnimationFileReaderStruct & ReaderObject, TSharedPtr<FJsonObject> JsonObject);
	static void GenerateStructsFromJson(struct FSAnimationFileReaderEvent & ReaderObject, TSharedPtr<FJsonObject> JsonObject);
	static void GenerateStructsFromJson(struct FSAnimationFileReaderAnimNotify & ReaderObject, TSharedPtr<FJsonObject> JsonObject);

	// Find class given name
	// e.g. FSEditorUtils::FindClass("MyAnimNotifyState");
	static UClass* FindClass(const FString& ClassName);

	static float FramesToTime(float Frames);

	//TEMPLATE Load Obj From Path
	template <typename ObjClass>
	static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
	{
		if(Path == NAME_None) return NULL;
		//~
	
		return Cast<ObjClass>(StaticLoadObject( ObjClass::StaticClass(), NULL,*Path.ToString()));
	}
	
private:
	static bool IsObjectAcceptable(UObject* Asset, const TArray<const UClass*>& AllowedClasses);
	static bool HasClassName(const FName& Name, const TArray<const UClass*>& AllowedClasses);
};
