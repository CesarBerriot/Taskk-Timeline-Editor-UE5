#ifdef UEUTILS_ONLINE_IMPLEMENTATION
#include "Online.nh"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "steam/steam_api.h"

#define NOT(x) (0 == (x))

using namespace UEUtils;

#pragma region Events
TDelegate<void(TSharedPtr<FOnlineSessionSearch> const &)> GOnLANSessionsSearchStarted = {};
TMulticastDelegate<void(APlayerController * const &)> GOnSessionMemberJoined = {};
TMulticastDelegate<void(AController * const &)> GOnSessionMemberLeft = {};
#pragma endregion Events

bool Online::ForceAsyncRefreshFriendList()
{
	if(IOnlineSubsystem const * const _onlineSubsystem = IOnlineSubsystem::Get())
		if(IOnlineFriendsPtr const _friendsInterface = _onlineSubsystem->GetFriendsInterface())
			return _friendsInterface->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default));
	return false;
}

TArray<TSharedRef<FOnlineFriend>> Online::GetAllFriends()
{
	TArray<TSharedRef<FOnlineFriend>> _friends = {};
	if(IOnlineFriendsPtr const _friendsInterface = IOnlineSubsystem::Get()->GetFriendsInterface())
		_friendsInterface->GetFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), _friends);
	return _friends;
}

TArray<TSharedRef<FOnlineFriend>> Online::GetAllOnlineFriends()
{
	TArray<TSharedRef<FOnlineFriend>> _friends = GetAllFriends();
	SteamWorksFunctions::RemoveOfflineFriendsFromList(_friends);
	return _friends;
}

bool Online::CreateSession()
{
	FOnlineSessionSettings _settings = {};
	_settings.bUsesPresence = true;
	_settings.bAllowInvites = true;
	_settings.bAllowJoinViaPresence = true;
	_settings.bUseLobbiesIfAvailable = true;
	_settings.bAllowJoinInProgress = true;
	_settings.bShouldAdvertise = true;
	_settings.NumPublicConnections = 4;
	_settings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	return IOnlineSubsystem::Get()->GetSessionInterface()->CreateSession(0, FName{ SESSION_NAME }, _settings);
}

bool Online::InviteFriend(FString _friendDisplayName)
{
	if(IOnlineSessionPtr const _sessionsInterface = IOnlineSubsystem::Get()->GetSessionInterface())
		for(TSharedRef<FOnlineFriend> const & _friend : GetAllFriends())
			if(_friend->GetDisplayName() == _friendDisplayName)
				return _sessionsInterface->SendSessionInviteToFriend(0, SESSION_NAME, _friend->GetUserId().Get());
	return false;
}

void Online::JoinLANSession()
{
	if(IOnlineSubsystem::Get()->GetSubsystemName() == "Steam")
		return;

	IOnlineSessionPtr _sessions = IOnlineSubsystem::Get()->GetSessionInterface();

	TSharedPtr<FOnlineSessionSearch> _searchResults = MakeShareable(new FOnlineSessionSearch());
	_searchResults->MaxSearchResults = 50;
	_searchResults->bIsLanQuery = true;

	if(_sessions->FindSessions(0, _searchResults.ToSharedRef()))
		GOnLANSessionsSearchStarted.ExecuteIfBound(_searchResults);
}

TArray<TSharedRef<FOnlineFriend>> Online::GetAllSessionMembersButHost()
{
	IOnlineSessionPtr const _sessionsInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if(!_sessionsInterface)
		return {};
	TArray<TSharedRef<FOnlineFriend>> _sessionMembers = GetAllFriends();
	for(int iFriendInAnySession = 0; iFriendInAnySession < _sessionMembers.Num();)
		if
		(
			TSharedRef<FOnlineFriend> const & _friend = _sessionMembers[iFriendInAnySession];
			_sessionsInterface->IsPlayerInSession(SESSION_NAME, _friend->GetUserId().Get())
		)
			++iFriendInAnySession;
		else
			_sessionMembers.RemoveAt(iFriendInAnySession);
	return _sessionMembers;
}

uint64 Online::SteamWorksFunctions::GetLobbyID()
{
	if(IOnlineSessionPtr const _sessionsInterface = IOnlineSubsystem::Get()->GetSessionInterface(); _sessionsInterface.Get())
		if(FNamedOnlineSession const * const _session = _sessionsInterface->GetNamedSession(UEUtils::Online::SESSION_NAME))
			if(auto const & _sessionInfo = _session->SessionInfo; _sessionInfo.IsValid())
				if(FUniqueNetIdRepl const & _sessionID = _sessionInfo->GetSessionId(); _sessionID.IsValid())
					return _wtoi64(*_sessionID.ToString());
	return NULL;
}

bool Online::SteamWorksFunctions::IsIDValid(uint64 const & _id) { return CSteamID(_id).IsValid(); }

uint64 Online::SteamWorksFunctions::ToSteamID(FUniqueNetIdRepl const & _netID)
{
	FString const _str = _netID->ToString();
	for(WIDECHAR _char : _str)
		if(!isdigit(_char))
			return NULL;
	return _wtoi64(*_str);
}

TDelegate<void(TSharedPtr<FOnlineSessionSearch> const &)> & Online::Events::OnLANSessionsSearchStarted() { return GOnLANSessionsSearchStarted; }

TMulticastDelegate<void(APlayerController * const &)> & Online::Events::OnSessionMemberJoined() { return GOnSessionMemberJoined; }

TMulticastDelegate<void(AController * const &)> & Online::Events::OnSessionMemberLeft() { return GOnSessionMemberLeft; }

TArray<uint64> Online::SteamWorksFunctions::GetAllFriends()
{
	return GetAllFriendsWithFlags(k_EFriendFlagImmediate);
}

TArray<uint64> Online::SteamWorksFunctions::GetAllOnlineFriends()
{
	TArray<uint64> _friends = GetAllFriends();
	for(int iFriend = 0; iFriend < _friends.Num();)
	{
		if(SteamFriends()->GetFriendPersonaState(_friends[iFriend]))
			++iFriend;
		else
			_friends.RemoveAt(iFriend);
	}
	return _friends;
}

TArray<uint64> Online::SteamWorksFunctions::GetAllSessionMembers()
{
	CSteamID _lobbyID = GetLobbyID();
	if(!_lobbyID.IsValid())
		return {};
	ISteamMatchmaking * const _steamMatchmaking = SteamMatchmaking();
	if(!_steamMatchmaking)
		return {};
	TArray<uint64> _members = {};
	int const _membersNum = _steamMatchmaking->GetNumLobbyMembers(_lobbyID);
	for(int iMember = 0; iMember < _membersNum; ++iMember)
		_members.Add(_steamMatchmaking->GetLobbyMemberByIndex(_lobbyID, iMember).ConvertToUint64());
	return _members;
}

FString Online::SteamWorksFunctions::GetFriendUsername(uint64 const & _friendID)
{
	ISteamFriends * _steamFriends;
	if(NOT(_steamFriends = SteamFriends()))
		return {};
	if(_friendID == SteamUser()->GetSteamID().ConvertToUint64())
		return UTF8_TO_TCHAR(_steamFriends->GetPersonaName());
	return UTF8_TO_TCHAR(_steamFriends->GetFriendPersonaName(_friendID));
}

TArray<uint64> Online::SteamWorksFunctions::GetAllFriendsWithFlags(int const & _flags)
{
	ISteamFriends * _steamFriends;
	if(NOT(_steamFriends = SteamFriends()))
		return {};
	TArray<uint64> _result = {};
	int32 const _count = _steamFriends->GetFriendCount(_flags);
	for(int32 iFriend = 0; iFriend < _count; ++iFriend)
	{
		CSteamID const _friendID = _steamFriends->GetFriendByIndex(iFriend, _flags);
		_result.Add(_friendID.ConvertToUint64());
	}
	return _result;
}

void Online::SteamWorksFunctions::RemoveOfflineFriendsFromList(TArray<TSharedRef<FOnlineFriend>> & _list)
{
	TArray<FString> _friendNames = {};
	{
		TArray<uint64> const _onlineFriends = GetAllOnlineFriends();
		for(uint64 const & _friend : _onlineFriends)
			_friendNames.Add(GetFriendUsername(_friend));
	}
	for(int iFriend = 0; iFriend < _list.Num();)
	{
		if(_friendNames.Contains(_list[iFriend]->GetDisplayName()))
			++iFriend;
		else
			_list.RemoveAt(iFriend);
	}
}
#endif /*UEUTILS_ONLINE_IMPLEMENTATION*/
