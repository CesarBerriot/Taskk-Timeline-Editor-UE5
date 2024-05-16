#pragma once

namespace PartyGameUtils::Template
{
#pragma region Private Members Access
	/*
	 * USAGE :
	 * 
	 * class SomeClass
	 * {
	 * private:
	 * 		int value = 4;
	 * };
	 * 	
	 * template struct TDeclarePrivateOffsetAccess<&SomeClass::value, class SomeClassValue>;
	 * //alternatively DECLARE_PRIVATE_OFFSET(SomeClass, value)
	 * 	 
	 * int main()
	 * {
	 *		SomeClass c = {};
	 *	 	int value = c.*(TGetPrivateOffset<int SomeClass::*, SomeClassValue>::Offset);
	 *	 	//alternatively int value = GET_PRIVATE_MEMBER(SomeClass, value, int, &c);
	 * }
	 */
	template <class OffsetType, class Key>
	struct TGetPrivateOffset
	{
	public:
		static inline OffsetType Offset = {};
	};

	template <auto OffsetVar, class Key>
	struct TDeclarePrivateOffsetAccess
	{
	private:
		static inline auto x = TGetPrivateOffset<decltype(OffsetVar), Key>::Offset = OffsetVar;
	};

#define DECLARE_PRIVATE_OFFSET(Class, Member) template struct PartyGameUtils::Template::TDeclarePrivateOffsetAccess<&Class::Member, class Class##Member##PrivateOffsetKey>;
#define GET_PRIVATE_MEMBER(Class, Member, MemberType, ClassInstancePtr) (ClassInstancePtr->*(PartyGameUtils::Template::TGetPrivateOffset<MemberType Class::*, Class##Member##PrivateOffsetKey>::Offset))
#pragma region Private Members Access
#pragma region TRemoveFuncAttributes
	template <typename T> struct TRemoveFuncAttributes
	{
		typedef T Type;
	};
	template <typename T, typename... TArgs> struct TRemoveFuncAttributes<T(TArgs...)>
	{
		typedef T Type;
	};
	template <typename T, typename... TArgs> struct TRemoveFuncAttributes<T(TArgs...) const>
	{
		typedef T Type;
	};
	template <typename T, typename... TArgs> struct TRemoveFuncAttributes<T(*)(TArgs...)>
	{
		typedef T Type;
	};
	template <typename T, class TClass, typename... TArgs> struct TRemoveFuncAttributes<T(TClass::*)(TArgs...)>
	{
		typedef T Type;
	};
	template <typename T, class TClass, typename... TArgs> struct TRemoveFuncAttributes<T(TClass::*)(TArgs...) const>
	{
		typedef T Type;
	};
#pragma endregion TRemoveFuncAttributes
}
