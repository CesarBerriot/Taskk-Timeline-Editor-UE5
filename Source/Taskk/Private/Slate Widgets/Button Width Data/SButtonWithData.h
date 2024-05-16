#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

template <typename DataType>
class TASKK_API SButtonWithData : public SCompoundWidget
{
public:
	typedef TDelegate<void(DataType)> FOnClicked;

	SLATE_BEGIN_ARGS(SButtonWithData) : _Text() {}
		SLATE_ARGUMENT(FText, Text)
		SLATE_EVENT(FOnClicked, OnClicked)
	SLATE_END_ARGS()

	void Construct(FArguments const & InArgs, DataType InData);
	TSharedPtr<SButton> Button;
	DataType Data;
private:
	FOnClicked OnClicked;
};

template <typename DataType> void SButtonWithData<DataType>::Construct(FArguments const & InArgs, DataType InData)
{
	Data = InData;
	OnClicked = InArgs._OnClicked;
	ChildSlot
	[
		SNew(SButton)
		.Text(InArgs._Text)
		.OnClicked_Lambda([this]
		{
			OnClicked.ExecuteIfBound(Data);
			return FReply::Handled();
		})
	];
}
