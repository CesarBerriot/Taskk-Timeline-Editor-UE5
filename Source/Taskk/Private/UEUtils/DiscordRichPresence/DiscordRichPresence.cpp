#ifdef UEUTILS_DISCORD_RICH_PRESENCE_IMPLEMENTATION
#include "DiscordRichPresence.h"
#include "discord_rpc.h"
#include "discord_register.h"

struct
{
	FTSTicker::FDelegateHandle TickerDelegateHandle;
	DiscordRichPresence RichPresence;
	bool Initialized;
} UnitGlobals = { {}, {}, false };


void HandleDiscordError(int ErrorCode, char const * Message)
{
	checkf(
		0,
		L""
		"UEUtils : Received an error from the Discord Rich Presence API. "
		"Error code : %i. "
		"Error Message : %hs.",
		ErrorCode,
		Message
	)
}

void UEUtils::DiscordRichPresence::InitDRP()
{
	checkf(!UnitGlobals.Initialized, L"UEUtils::DiscordRichPresence has already been initialized")

	DiscordEventHandlers EventHandlers;
	memset(&EventHandlers, 0, sizeof(DiscordEventHandlers));
	EventHandlers.errored = HandleDiscordError;
	Discord_Initialize(UEUTILS_DISCORD_RICH_PRESENCE_APP_ID, &EventHandlers, false, nullptr);

	FTickerDelegate TickerDelegate = {};
	TickerDelegate.BindLambda([](float)
	{
		Discord_RunCallbacks();
		return true;
	});
	UnitGlobals.TickerDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickerDelegate);

	memset(&UnitGlobals.RichPresence, 0, sizeof(::DiscordRichPresence));

	UnitGlobals.Initialized = true;
}

void UEUtils::DiscordRichPresence::ShutdownDRP()
{
	checkf(UnitGlobals.Initialized, L"UEUtils::DiscordRichPresence hasn't been initialized")
	FTSTicker::GetCoreTicker().RemoveTicker(UnitGlobals.TickerDelegateHandle);
	UnitGlobals.TickerDelegateHandle.Reset();
	Discord_Shutdown();
	free((void*)UnitGlobals.RichPresence.state);
	free((void*)UnitGlobals.RichPresence.details);
	free((void*)UnitGlobals.RichPresence.largeImageKey);
	free((void*)UnitGlobals.RichPresence.largeImageText);
	free((void*)UnitGlobals.RichPresence.smallImageKey);
	free((void*)UnitGlobals.RichPresence.smallImageText);
	UnitGlobals.Initialized = false;
}

void UEUtils::DiscordRichPresence::RefreshRichPresence()
{
	check(UnitGlobals.Initialized)
	Discord_UpdatePresence(&UnitGlobals.RichPresence);
}

void UEUtils::DiscordRichPresence::ClearRichPresence()
{
	check(UnitGlobals.Initialized)
	Discord_ClearPresence();
}

#define DEFINE_RICH_PRESENCE_STRING_SETTER(VariableName, VariableNameLowerCase) void UEUtils::DiscordRichPresence::SetRP##VariableName(FString VariableName) { check(UnitGlobals.Initialized) free((void*)UnitGlobals.RichPresence.VariableNameLowerCase); UnitGlobals.RichPresence.VariableNameLowerCase = strdup(TCHAR_TO_ANSI(*VariableName)); }
DEFINE_RICH_PRESENCE_STRING_SETTER(State, state)
DEFINE_RICH_PRESENCE_STRING_SETTER(Details, details)
DEFINE_RICH_PRESENCE_STRING_SETTER(LargeImageKey, largeImageKey)
DEFINE_RICH_PRESENCE_STRING_SETTER(LargeImageText, largeImageText)
DEFINE_RICH_PRESENCE_STRING_SETTER(SmallImageKey, smallImageKey)
DEFINE_RICH_PRESENCE_STRING_SETTER(SmallImageText, smallImageText)

void UEUtils::DiscordRichPresence::SetRPStartTimestamp(int64 StartTimestamp)
{
	check(UnitGlobals.Initialized)
	UnitGlobals.RichPresence.startTimestamp = StartTimestamp;
}

void UEUtils::DiscordRichPresence::SetRPEndTimestamp(int64 EndTimestamp)
{
	check(UnitGlobals.Initialized)
	UnitGlobals.RichPresence.endTimestamp = EndTimestamp;
}
#endif // UEUTILS_DISCORD_RICH_PRESENCE_IMPLEMENTATION
