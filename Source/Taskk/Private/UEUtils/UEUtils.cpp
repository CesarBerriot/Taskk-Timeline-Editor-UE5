#include "UEUtils.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

UEnhancedInputLocalPlayerSubsystem * UEUtils::GetEnhancedInputLocalPlayerSubsystem(UWorld const * const & _world)
{
	if(_world)
		if(APlayerController const * const _pc = _world->GetFirstPlayerController())
			return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_pc->GetLocalPlayer());
	return nullptr;
}

void UEUtils::QuitGame(UWorld const * const& _world)
{
	if(_world)
		UKismetSystemLibrary::QuitGame(_world, _world->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
