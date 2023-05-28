#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FSoulsGameEditorModule : public IModuleInterface
{
public:
	// Begin IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void ExtendMenu();


	// End IModuleInterface implementation
	static FSoulsGameEditorModule & Get();

private:

	UWorld* GetEditorWorld() const;
	static FSoulsGameEditorModule * SEditorInstance;
	void BindMenuCommands();
	void AddEditorMenu(FMenuBarBuilder& MenuBarBuilder);
	void AddMainMenuExtension(FMenuBuilder & MenuBuilder);

	void RegisterDetails();
	void UnregisterDetails();

	void OnPIE(const bool bIsSimulating);

	// List of UI commands used by the various menus
	TSharedPtr<class FUICommandList> SCommands;

	// The extender to pass to the level editor to extend it's File menu.
	TSharedPtr<FExtender> MainMenuExtender;
};