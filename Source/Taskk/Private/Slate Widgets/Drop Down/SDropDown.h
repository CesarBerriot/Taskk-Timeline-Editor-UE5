#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * @brief A standard dropdown widget
 * @tparam UserDataType Type used by **Userdata**, which optionally holds whatever data you give it 
 */
template <typename UserDataType = int>
class TASKK_API SDropDown : public SCompoundWidget
{
public:
	typedef TDelegate<void(bool IsDropped, TAttribute<UserDataType> UserData)> FOnToggled;

	SLATE_BEGIN_ARGS(SDropDown)
		: _BarContent()
		, _Name("Unnamed DropDown")
		, _IsDropped(false)
		, _LeftPadding(0)
		, _UserData() {}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_NAMED_SLOT(FArguments, BarContent)
		SLATE_ATTRIBUTE(FName, Name)
		SLATE_ARGUMENT(bool, IsDropped)
		SLATE_ARGUMENT(float, LeftPadding)
		SLATE_ATTRIBUTE(UserDataType, UserData)
		SLATE_EVENT(FOnToggled, OnToggled)
	SLATE_END_ARGS()

	void Construct(FArguments const & InArgs);
private:
	TSharedRef<SWidget> ConstructDropDownBar(TAttribute<FName> Name, TAlwaysValidWidget BarContent);
	/// Refreshes both the v/< text and the widget switcher
	void Refresh();
	bool IsDropped;
	TSharedPtr<SWidget> Content;
	TSharedPtr<SBorder> Border;
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
	TSharedPtr<STextBlock> DropDownArrowTextBox;
	FOnToggled OnToggled;
	TAttribute<UserDataType> UserData;
};

#include "SDropDownDefinitions.inl"
