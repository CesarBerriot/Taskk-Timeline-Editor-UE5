#include "SlateOptMacros.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

template <typename DataType>
void SDropDown<DataType>::Construct(FArguments const& InArgs)
{
	OnToggled = InArgs._OnToggled;
	Content = InArgs._Content.Widget;
	IsDropped = InArgs._IsDropped;
	UserData = InArgs._UserData;
	ChildSlot
	[
		SAssignNew(Border, SBorder)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()[ConstructDropDownBar(InArgs._Name, InArgs._BarContent)]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SBox)
				.Padding(InArgs._LeftPadding, 0, 0, 0)
				[
					SAssignNew(WidgetSwitcher, SWidgetSwitcher).WidgetIndex(0)
				]
			]
		]
	];
	WidgetSwitcher->AddSlot(false)[SNullWidget::NullWidget];
	WidgetSwitcher->AddSlot(true)[Content.ToSharedRef()];
	Refresh();
}

template <typename DataType>
TSharedRef<SWidget> SDropDown<DataType>::ConstructDropDownBar(TAttribute<FName> Name, TAlwaysValidWidget BarContent)
{
	TSharedPtr<SWidget> ContentWidget = BarContent.Widget;
	if (BarContent.Widget == SNullWidget::NullWidget && Name.IsSet())
		SAssignNew(ContentWidget, STextBlock).Text(FText::FromName(Name.Get()));
	// @formatter:off
	return
	SNew(SButton)
	.OnClicked_Lambda([this] { IsDropped = !IsDropped; Refresh(); OnToggled.ExecuteIfBound(IsDropped, UserData); return FReply::Handled(); })
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		[
			ContentWidget.ToSharedRef()
		]
		+ SHorizontalBox::Slot()
		.Padding(5, 0)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Center)
			  .FillHeight(1)
			[
				SNew(SSeparator)
				.ColorAndOpacity(FColorList::White)
				.Orientation(Orient_Horizontal)
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(DropDownArrowTextBox, STextBlock)
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / L"Slate/Fonts/Roboto-Bold.ttf", 12))
			.Text(FText::FromString("TODO REMOVE"))
		]
	];
	// @formatter:on
}

template <typename DataType>
void SDropDown<DataType>::Refresh()
{
	Border->SetBorderBackgroundColor(IsDropped ? FLinearColor::White : FLinearColor::Transparent);
	DropDownArrowTextBox->SetText(FText::FromName(IsDropped ? L"-" : L"+"));
	WidgetSwitcher->SetActiveWidgetIndex(IsDropped);
}
