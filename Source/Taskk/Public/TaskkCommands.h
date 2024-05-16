// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TaskkStyle.h"

class FTaskkCommands : public TCommands<FTaskkCommands>
{
public:

	FTaskkCommands()
		: TCommands<FTaskkCommands>(TEXT("Taskk"), NSLOCTEXT("Contexts", "Taskk", "Taskk Plugin"), NAME_None, FTaskkStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};