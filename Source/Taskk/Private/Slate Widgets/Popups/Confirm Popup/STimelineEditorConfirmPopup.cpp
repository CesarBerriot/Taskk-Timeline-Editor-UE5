// Fill out your copyright notice in the Description page of Project Settings.


#include "STimelineEditorConfirmPopup.h"

#include "SlateOptMacros.h"
#include "Name Space/TimelineEditor.h"
#include "Widgets/Layout/SScrollBox.h"

STimelineEditorConfirmPopup::~STimelineEditorConfirmPopup() { OnExited.ExecuteIfBound(Confirmed); }

void STimelineEditorConfirmPopup::Construct(FArguments const & InArgs)
{
	OnExited = InArgs._OnExited;
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			+ SScrollBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1)
				  .HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(InArgs._Text)
				]
			]
		]
		+ SVerticalBox::Slot()
		  .Padding(0, 8, 0, 0)
		  .AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromName("Cancel"))
				.HAlign(HAlign_Center)
				.OnClicked_Raw(this, &STimelineEditorConfirmPopup::Cancel)
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromName("Confirm"))
				.HAlign(HAlign_Center)
				.OnClicked_Raw(this, &STimelineEditorConfirmPopup::Confirm)
			]
		]
	];
}

FReply STimelineEditorConfirmPopup::Cancel()
{
	TimelineEditor::DestroyLastPopup();
	return FReply::Handled();
}

FReply STimelineEditorConfirmPopup::Confirm()
{
	Confirmed = true;
	TimelineEditor::DestroyLastPopup();
	return FReply::Handled();
}
