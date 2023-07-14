#include "SGEditorInfoObject.h"

void USGEditorInfoObject::SetInputObject(UObject* Other)
{
	InputObject = Other;
}

void USGEditorInfoObject::SetJsonObjectPath(FString Path)
{
	JsonObjectPath = Path;
}
