#pragma once

#pragma region Assignment Conditions
// epic games go fuck yourself
#define ifa(x) if((bool)(x))
#define whilea(x) while((bool)(x))
#define fora(a, b, c) for(a; (bool)(c); c)
#define asserta(x) assert((bool)(x))
#define checka(x) check((bool)(x))
#define checkaf(x, ...) check((bool)(x), __VA_ARGS__)
#define verifya(x) verify((bool)(x))
#define verifyaf(x, ...) verifyf((bool)(x), __VA_ARGS__)
#define ensurea(x) ensure((bool)(x))
#define ensureaMsgf(x, ...) ensureMsgf((bool)(x), __VA_ARGS__)
#pragma endregion Assignment Conditions

#pragma region Template Conditions
//TODO move this to the Templates namespace and make an actual type
#define TIsUObjectPtr(x)\
TIsDerivedFrom\
<\
	TRemoveCV\
	<\
		TRemovePointer\
		<\
			TRemoveCV\
			<\
				TRemoveReference<decltype(x)>::Type\
			>::Type\
		>::Type\
	>::Type,\
	UObject\
>
#pragma endregion Template Conditions

#pragma region Checks

#define checkProperty(Property) checkf((TIsUObjectPtr(Property)::Value) ? IsValid((UObject const * const &)Property) : (bool)Property, L"property '%hs' in instance of class '%ls' was invalid !", #Property, *ThisClass::StaticClass()->GetName())

#define checkPtr(Ptr) checkf((TIsUObjectPtr(Ptr)::Value) ? IsValid((UObject const * const &)Ptr) : (bool)Ptr, L"pointer '%hs' in instance of class '%ls' was invalid !", #Ptr, *ThisClass::StaticClass()->GetName())

#define checkIsBlueprint() checkf(GetClass() != ThisClass::StaticClass(), L"class %ls Instance Initialization : failed IsBlueprint check", *ThisClass::StaticClass()->GetName());
#define checkIsWidgetBlueprint() checkf(GetClass() != ThisClass::StaticClass(), L"%ls::NativeConstruct : failed IsBlueprint check", *ThisClass::StaticClass()->GetName());
#define checkIsActorBlueprint() checkf(GetClass() != ThisClass::StaticClass(), L"%ls::BeginPlay : failed IsBlueprint check", *ThisClass::StaticClass()->GetName());
#define checkIsGameInstanceBlueprint() checkf(GetClass() != ThisClass::StaticClass(), L"%ls::Init : failed IsBlueprint check", *ThisClass::StaticClass()->GetName());
#define checkIsGameStateBlueprint checkIsActorBlueprint
#define checkIsGameModeBaseBlueprint() checkf(GetClass() != ThisClass::StaticClass(), L"%ls::InitGame : failed IsBlueprint check", *ThisClass::StaticClass()->GetName());
#pragma endregion Checks

#define DEFINE_INLINE_MEMBER_GETTER(NameLowerCasePrefix, NameUpperCasePrefix, Name) FORCEINLINE decltype(NameLowerCasePrefix##Name) const & Get##NameUpperCasePrefix##Name() const { return  NameLowerCasePrefix##Name; }


#pragma region Log
#if !UE_BUILD_SHIPPING
	#define __get_game_instance PartyGameUtils::GetGameInstanceChecked(GetWorld())
	
	#define LOG __get_game_instance.Log
	#define LOGF(Color, Format, ...) __get_game_instance.Logf(ELogColor::Color, Format, __VA_ARGS__)
	#define LOG_SEPARATOR __get_game_instance.LogSeparator
	
	#define __get_game_instance_custom_world(World) PartyGameUtils::GetGameInstanceChecked(World)
	
	#define LOG_EX(World, ...) __get_game_instance_custom_world(World).Log(__VA_ARGS__)
	#define LOGF_EX(World, Color, Format, ...) __get_game_instance_custom_world(World).Logf(ELogColor::Color, Format, __VA_ARGS__)
	#define LOG_SEPARATOR_EX(World, ...) __get_game_instance_custom_world(World).LogSeparator(__VA_ARGS__)
#else
	#define LOG(...) 				0
	#define LOGF(...)				0
	#define LOG_SEPARATOR(...)		0
	#define LOG_EX(...)				0
	#define LOGF_EX(...)			0
	#define LOG_SEPARATOR_EX(...)	0
#endif
#pragma endregion Log
#pragma region IF_DEFINED
// IF_UE_BUILD_SHIPPING(x, y) expands to x if UE_BUILD_SHIPPING is set, otherwise expands to y
#if UE_BUILD_SHIPPING
	#define IF_UE_BUILD_SHIPPING(x, y) x
#else
	#define IF_UE_BUILD_SHIPPING(x, y) y
#endif
#pragma endregion IF_DEFINED
