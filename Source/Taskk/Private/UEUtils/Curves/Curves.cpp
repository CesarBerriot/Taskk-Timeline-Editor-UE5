#include "Curves.h"

float UEUtils::Curves::EaseInOutSine(float const & _delta)
{
	return -(FMath::Cos(UE_PI * _delta) - 1.f) / 2.f;
}

float UEUtils::Curves::EaseOutCirc(float const & _delta)
{
	return FMath::Sqrt(1.f - FMath::Pow(_delta - 1.f, 2.f));
}
