#include "SEditorInfoObject.h"

void USEditorInfoObject::SetInputObject(UObject* Other)
{
	InputObject = Other;
}

void USEditorInfoObject::SetJsonObjectPath(FString Path)
{
	JsonObjectPath = Path;
}
