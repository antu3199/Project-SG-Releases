#pragma once
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

// Class containing commands for Houdini Engine actions
class FSCommands : public TCommands<FSCommands>
{
public:
	FSCommands();

	// TCommand<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> _OpenSGWindow;

	static void OpenSGWindow();
	static TSharedRef<class SVerticalBox> CreateSGContent();
	static TArray<const UClass*> GetAllowedClasses();
	static bool IsObjectAcceptable(UObject* Asset);
	static bool HasClassName(const FName& Name);

	static class USGameSingleton* GetGameSingleton();
	static class USEditorInfoObject* GetInfoObject();
	static void ForceUpdateWindow();

private:
	static void OpenWindow(TSharedPtr<SWindow> Window);
	static TSharedPtr<SWindow> SGWindow;
};

