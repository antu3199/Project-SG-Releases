#include "SoulsGameEditor/Public/SoulsGameEditorModule.h"

#include "IAssetViewport.h"
#include "LevelEditor.h"
#include "SGAssetActorDetails.h"
#include "SGAssetComponentDetails.h"
#include "SGCommands.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/SGGameSingleton.h"
#include "Subsystems/UnrealEditorSubsystem.h"

IMPLEMENT_GAME_MODULE(FSoulsGameEditorModule, SoulsGameEditor);

#define LOCTEXT_NAMESPACE "FSoulsGameEditorModule"
FSoulsGameEditorModule *
FSoulsGameEditorModule::SEditorInstance = nullptr;


void FSoulsGameEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("StartupModule() called"));

	FSoulsGameEditorModule::SEditorInstance = this;

	// Extends the file menu.
	ExtendMenu();

	// Register detail presenters.
	RegisterDetails();

	FEditorDelegates::PreBeginPIE.AddRaw(this, &FSoulsGameEditorModule::OnPIE);
	
}

void FSoulsGameEditorModule::ShutdownModule()
{
	FEditorDelegates::PreBeginPIE.RemoveAll(this);
	UE_LOG(LogTemp, Warning, TEXT("ShutdownModule() called"));
	UnregisterDetails();
}


FSoulsGameEditorModule& FSoulsGameEditorModule::Get()
{
	return *SEditorInstance;
}

UWorld* FSoulsGameEditorModule::GetEditorWorld() const
{
	UUnrealEditorSubsystem* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
	return UnrealEditorSubsystem ? UnrealEditorSubsystem->GetEditorWorld() : nullptr;
}

void FSoulsGameEditorModule::ExtendMenu()
{
	if (IsRunningCommandlet())
		return;

	// We need to add/bind the UI Commands to their functions first
	BindMenuCommands();

	MainMenuExtender = MakeShareable(new FExtender);
	MainMenuExtender->AddMenuBarExtension(
		"Edit",
		EExtensionHook::After,
		SCommands,
		FMenuBarExtensionDelegate::CreateRaw(this, &FSoulsGameEditorModule::AddEditorMenu));

	
	// Add our menu extender
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MainMenuExtender);
}

void
FSoulsGameEditorModule::BindMenuCommands()
{
	SCommands = MakeShareable(new FUICommandList);

	FSGCommands::Register();
	const FSGCommands& Commands = FSGCommands::Get();
	

	SCommands->MapAction(
		Commands._OpenSGWindow,
		FExecuteAction::CreateLambda([]() {FSGCommands::OpenSGWindow(); }),
		FCanExecuteAction::CreateLambda([]() { return true; }));


	// Append the command to the editor module
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked< FLevelEditorModule >("LevelEditor");
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(SCommands.ToSharedRef());
}

void FSoulsGameEditorModule::AddEditorMenu(FMenuBarBuilder& MenuBarBuilder)
{
	// View
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("SGLabel", "SGMenu"),
		LOCTEXT("SGMenu_ToolTip", "Open the SG Menu"),
		FNewMenuDelegate::CreateRaw(this, &FSoulsGameEditorModule::AddMainMenuExtension),
		"View");
}

void
FSoulsGameEditorModule::AddMainMenuExtension(FMenuBuilder & MenuBuilder)
{
	MenuBuilder.BeginSection("Actions", LOCTEXT("ActionsLabel", "Actions"));
	MenuBuilder.AddMenuEntry(FSGCommands::Get()._OpenSGWindow);
	MenuBuilder.EndSection();
}

void FSoulsGameEditorModule::RegisterDetails()
{
	FPropertyEditorModule & PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");

	// Register details presenter for our component type and runtime settings.
	PropertyModule.RegisterCustomClassLayout(
		TEXT("SAssetComponent"),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSGAssetComponentDetails::MakeInstance));

	PropertyModule.RegisterCustomClassLayout(
		TEXT("SAssetActor"),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSGAssetActorDetails::MakeInstance));
}

void FSoulsGameEditorModule::UnregisterDetails()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule & PropertyModule =
			FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.UnregisterCustomClassLayout(TEXT("SAssetComponent"));
		PropertyModule.UnregisterCustomClassLayout(TEXT("SAssetActor"));
	}
}

void FSoulsGameEditorModule::OnPIE(const bool bIsSimulating)
{
	// Allow for "Play from here"
	if (UWorld* World = GetEditorWorld())
	{
		if (USGGameSingleton* Singleton = Cast<USGGameSingleton>(GEditor->GameSingleton))
		{
			Singleton->OnPIE(World);

			/*
			// Need to check if it is a UI map
			FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
			TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();
			// Make sure we can find a path to the view port.  This will fail in cases where the view port widget
			// is in a backgrounded tab, etc.  We can't currently support starting PIE in a backgrounded tab
			// due to how PIE manages focus and requires event forwarding from the application.
			if (ActiveLevelViewport.IsValid() && FSlateApplication::Get().FindWidgetWindow(ActiveLevelViewport->AsWidget()).IsValid())
			{
				Singleton->bHasWorldPlacement = false;
				return;
			}
			*/
			
			if (GUnrealEd != nullptr)
			{
				const TOptional<FPlayInEditorSessionInfo> SessionInfo = GUnrealEd->GetPlayInEditorSessionInfo();
				Singleton->bHasWorldPlacement = SessionInfo.IsSet() && SessionInfo->OriginalRequestParams.StartLocation.IsSet();
			}
		}
	}
	//int32 Test = 0;
	
}
