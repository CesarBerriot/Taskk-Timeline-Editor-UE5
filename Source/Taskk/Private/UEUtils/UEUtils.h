#pragma once

#include "DefaultTemplateTypes.h"

UE_PUSH_MACRO("TEMPLATE_DEFAULT")
UE_PUSH_MACRO("ARG_WORLD")
UE_PUSH_MACRO("DEFINE_SIMPLE_GETTER")
UE_PUSH_MACRO("TCAST")

#define TEMPLATE_DEFAULT template <class T>
#define ARG_WORLD UWorld const * const & _world

class UEnhancedInputLocalPlayerSubsystem;

#include "Internal/Shared/PushAndDefineDeclareInlineCheckedGetter.def"

namespace UEUtils
{
	template <class TObjectClass, typename... TArgs> FORCEINLINE TObjectClass & GetChecked(TObjectClass * const & _object, WIDECHAR const * const & _checkMsg, TArgs... _args);
	UEnhancedInputLocalPlayerSubsystem * GetEnhancedInputLocalPlayerSubsystem(ARG_WORLD);
	template <class T = UEUtilsDefaultGMType> FORCEINLINE T * GetGameMode(ARG_WORLD);
	template <class T = UEUtilsDefaultGSType> FORCEINLINE T * GetGameState(ARG_WORLD);
	template <class T = UEUtilsDefaultGameInstanceType> FORCEINLINE T * GetGameInstance(ARG_WORLD);
	template <class T = UEUtilsDefaultHUDType> FORCEINLINE T * GetHUD(ARG_WORLD);
	template <class T = APawn> FORCEINLINE T * GetPlayer(ARG_WORLD);
	template <class T = UEUtilsDefaultGMType> DEFINE_INLINE_CHECKED_GETTER(T, GameMode, <T>)
	template <class T = UEUtilsDefaultGSType> DEFINE_INLINE_CHECKED_GETTER(T, GameState, <T>)
	template <class T = UEUtilsDefaultGameInstanceType> DEFINE_INLINE_CHECKED_GETTER(T, GameInstance, <T>)
	template <class T = UEUtilsDefaultHUDType> DEFINE_INLINE_CHECKED_GETTER(T, HUD, <T>)
	template <class T = APawn> DEFINE_INLINE_CHECKED_GETTER(T, Player, <T>)
	DEFINE_INLINE_CHECKED_GETTER(UEnhancedInputLocalPlayerSubsystem, EnhancedInputLocalPlayerSubsystem)
	void QuitGame(UWorld const * const & _world);
};

#include "Internal/Shared/PopDeclareInlineCheckedGetter.def"

template <class TObjectClass, typename... TArgs> FORCEINLINE TObjectClass & UEUtils::GetChecked(TObjectClass * const & _object, WIDECHAR const * const & _checkMsg, TArgs... _args)
{
	checkf((TIsDerivedFrom<TObjectClass, UObject>::Value ? IsValid((UObject const * const &)_object) : (bool)_object), _checkMsg, _args...)
	return *_object;
}

#define DEFINE_SIMPLE_GETTER(Name, Class, /*Code*/...)\
TEMPLATE_DEFAULT FORCEINLINE T * UEUtils::Name(ARG_WORLD)\
{\
	static_assert(TIsDerivedFrom<T, Class>::Value);\
	if(_world)\
		__VA_ARGS__;\
	return nullptr;\
}

#define TCAST(x) Cast<T>(x)

DEFINE_SIMPLE_GETTER(GetGameMode, AGameModeBase, return TCAST(_world->GetAuthGameMode()))
DEFINE_SIMPLE_GETTER(GetGameState, AGameStateBase, return TCAST(_world->GetGameState()))
DEFINE_SIMPLE_GETTER(GetGameInstance, UGameInstance, return TCAST(_world->GetGameInstance()))
DEFINE_SIMPLE_GETTER(GetHUD, AHUD, if(APlayerController const * const _pc = _world->GetFirstPlayerController()) return TCAST(_pc->GetHUD()))
DEFINE_SIMPLE_GETTER(GetPlayer, APawn, if(APlayerController const * const _pc = _world->GetFirstPlayerController()) return TCAST(_pc->GetPawn()))

UE_POP_MACRO("TEMPLATE_DEFAULT")
UE_POP_MACRO("ARG_WORLD")
UE_POP_MACRO("DEFINE_SIMPLE_GETTER")
UE_POP_MACRO("TCAST")
