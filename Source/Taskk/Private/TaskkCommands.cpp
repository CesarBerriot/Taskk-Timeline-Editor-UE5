// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskkCommands.h"

#define LOCTEXT_NAMESPACE "FTaskkModule"

void FTaskkCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Taskk", "Bring up Taskk window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
