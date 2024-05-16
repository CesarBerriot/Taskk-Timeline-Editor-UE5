/**
 * Helpers for the Discord RPC API
 * Define UEUTILS_DISCORD_RICH_PRESENCE_IMPLEMENTATION to use
 * Define UEUTILS_DISCORD_RICH_PRESENCE_APP_ID to a string 
 */

#ifdef UEUTILS_DISCORD_RICH_PRESENCE_IMPLEMENTATION

#ifndef UEUTILS_DISCORD_RICH_PRESENCE_APP_ID
#error UEUtils : Discord rich presence support has been enabled but UEUTILS_DISCORD_RICH_PRESENCE_APP_ID isn't defined
#endif // UEUTILS_DISCORD_RICH_PRESENCE_APP_ID

#pragma once

namespace UEUtils::DiscordRichPresence
{
	void InitDRP();
	void ShutdownDRP();
	/// Note : There's a rate limit of one call every 15 seconds between calls set in DRP,
	/// non-conforming calls will be queued up by DRP and called back-to-back every 15 seconds
	void RefreshRichPresence();
	void ClearRichPresence();
	void SetRPState(FString State);
	void SetRPDetails(FString Details);
	void SetRPStartTimestamp(int64 StartTimestamp);
	void SetRPEndTimestamp(int64 EndTimestamp);
	void SetRPLargeImageKey(FString LargeImageKey);
	void SetRPLargeImageText(FString LargeImageText);
	void SetRPSmallImageKey(FString SmallImageKey);
	void SetRPSmallImageText(FString SmallImageText);
};
#endif // UEUTILS_DISCORD_RICH_PRESENCE_IMPLEMENTATION
