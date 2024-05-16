// Copyright Epic Games, Inc. All Rights Reserved.

#include "Taskk.h"
#include "TaskkStyle.h"
#include "TaskkCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
//#include "Slate Widgets/Main/STimelineEditor.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"

static const FName TaskkTabName("Taskk");

#define LOCTEXT_NAMESPACE "FTaskkModule"

void FTaskkModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTaskkStyle::Initialize();
	FTaskkStyle::ReloadTextures();

	FTaskkCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTaskkCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTaskkModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTaskkModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TaskkTabName, FOnSpawnTab::CreateRaw(this, &FTaskkModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTaskkTabTitle", "Taskk"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTaskkModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTaskkStyle::Shutdown();

	FTaskkCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TaskkTabName);
}

TSharedRef<SDockTab> FTaskkModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FTaskkModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Taskk.cpp"))
		);

	TSharedPtr<SWidget> TimelineEditorWidget;
	if(UTimelineEditorToolWidget::Singleton)
		TimelineEditorWidget = UTimelineEditorToolWidget::Singleton->TakeWidget();
	else
		TimelineEditorWidget = CreateWidget<UTimelineEditorToolWidget>(GWorld.GetReference())->TakeWidget();

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			TimelineEditorWidget.ToSharedRef()
		];
}

void FTaskkModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TaskkTabName);
}

void FTaskkModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTaskkCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTaskkCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTaskkModule, Taskk)