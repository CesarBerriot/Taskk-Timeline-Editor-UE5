#ifdef UEUTILS_ONLINE_IMPLEMENTATION
#pragma once

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

namespace UEUtils::Online
{
	constexpr inline WIDECHAR const * const SESSION_NAME = L"sample_session_name";
	FORCEINLINE FUniqueNetIdPtr GetSelfUniqueNetID(UWorld const * const & _world) { return _world->GetFirstPlayerController()->GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId(); }
	bool ForceAsyncRefreshFriendList();
	TArray<TSharedRef<FOnlineFriend>> GetAllFriends();
	TArray<TSharedRef<FOnlineFriend>> GetAllOnlineFriends();
	bool CreateSession();
	bool InviteFriend(FString _friendDisplayName);
	void JoinLANSession();
	//TODO FIX | BROKEN
	TArray<TSharedRef<FOnlineFriend>> GetAllSessionMembersButHost();
	namespace Events
	{
		TDelegate<void(TSharedPtr<FOnlineSessionSearch> const &)> & OnLANSessionsSearchStarted();
		TMulticastDelegate<void(APlayerController * const &)> & OnSessionMemberJoined();
		TMulticastDelegate<void(AController * const &)> & OnSessionMemberLeft();
		FORCEINLINE FOnSessionSettingsUpdated & OnSessionSettingsUpdated() { return IOnlineSubsystem::Get()->GetSessionInterface()->OnSessionSettingsUpdatedDelegates; }
		namespace ClientSide
		{
			FORCEINLINE FOnFindSessionsComplete & OnLANSessionFound() { return IOnlineSubsystem::Get()->GetSessionInterface()->OnFindSessionsCompleteDelegates; }
			FORCEINLINE FOnJoinSessionComplete & OnSessionJoined() { return IOnlineSubsystem::Get()->GetSessionInterface()->OnJoinSessionCompleteDelegates; }
			FORCEINLINE FOnSessionUserInviteAccepted & OnInviteAccepted() { return IOnlineSubsystem::Get()->GetSessionInterface()->OnSessionUserInviteAcceptedDelegates; }
		};
		namespace ServerSide
		{
			FORCEINLINE FOnCreateSessionComplete & OnSessionCreated() { return IOnlineSubsystem::Get()->GetSessionInterface()->OnCreateSessionCompleteDelegates; }
		}
	};
	namespace SteamWorksFunctions
	{
		uint64 GetLobbyID();
		bool IsIDValid(uint64 const & _id);
		uint64 ToSteamID(FUniqueNetIdRepl const & _netID);
		TArray<uint64> GetAllFriends();
		TArray<uint64> GetAllOnlineFriends();
		TArray<uint64> GetAllSessionMembers();
		FString GetFriendUsername(uint64 const & _friendID);
		TArray<uint64> GetAllFriendsWithFlags(int const & _flags);
		void RemoveOfflineFriendsFromList(TArray<TSharedRef<FOnlineFriend>> & _list);
	};
};
#endif /*UEUTILS_ONLINE_IMPLEMENTATION*/
