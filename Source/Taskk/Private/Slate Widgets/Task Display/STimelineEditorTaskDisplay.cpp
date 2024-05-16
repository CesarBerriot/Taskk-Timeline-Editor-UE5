// Fill out your copyright notice in the Description page of Project Settings.


#include "STimelineEditorTaskDisplay.h"

#include "SlateOptMacros.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Name Space/TimelineEditor.h"
#include "Slate Widgets/Drop Down/SDropDown.h"
#include "Slate Widgets/Main/STimelineEditor.h"
#include "Slate Widgets/Popups/Confirm Popup/STimelineEditorConfirmPopup.h"
#include "Slate Widgets/Popups/Task Editor/STimelineEditorTaskEditor.h"
#include "Widgets/Layout/SScrollBox.h"

void STimelineEditorTaskDisplay::Construct(FArguments const & InArgs, uint64 TaskID)
{
	DisplayedTaskID = TaskID;
	FTimelineTask Task = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[DisplayedTaskID];
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()[ConstructButtonPad()]
		+ SVerticalBox::Slot().AutoHeight()[ConstructMemberNameTextBox("Name")]
		+ SVerticalBox::Slot().AutoHeight()[ConstructHorizontalScrollableText(Task.Name.ToString())]
		+ SVerticalBox::Slot().Padding(0, 4)
		+ SVerticalBox::Slot().AutoHeight()[ConstructMemberNameTextBox("Members")]
		+ SVerticalBox::Slot().AutoHeight()[ConstructMemberListTextBox()]
		+ SVerticalBox::Slot().Padding(0, 4)
		+ SVerticalBox::Slot().AutoHeight()[ConstructMemberNameTextBox("Description")]
		+ SVerticalBox::Slot().AutoHeight()[ConstructHorizontalScrollableText(Task.Description)]
		+ SVerticalBox::Slot().Padding(0, 4)
		+ SVerticalBox::Slot().AutoHeight()[ConstructBottomSection()]
	];
}

TSharedRef<SWidget> STimelineEditorTaskDisplay::ConstructHorizontalScrollableWidget(TSharedRef<SWidget> Content)
{
	return
	SNew(SScrollBox)
	.Orientation(Orient_Horizontal)
	+ SScrollBox::Slot()
	.AutoSize()
	[
		Content
	];
}

TSharedRef<SWidget> STimelineEditorTaskDisplay::ConstructBottomSection()
{
	FTimelineTask Task = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[DisplayedTaskID];
	bool ShowSubtasks = (bool)Task.SubTasks.Num();
	TSharedPtr<SVerticalBox> SubTasksVerticalBox;
	// @formatter:off
	TSharedRef<SWidget> Result =
	SNew(SVerticalBox)
	+ SVerticalBox::Slot().AutoHeight()[ConstructMemberNameTextBox(ShowSubtasks ? "Subtasks" : "Finished")]
	+ (ShowSubtasks
		   ? SVerticalBox::Slot().AutoHeight()[SAssignNew(SubTasksVerticalBox, SVerticalBox)]
		   : SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   SNew(STextBlock).Text(FText::FromName(Task.Finished ? "Yes" : "No"))
			   ]
			   + SHorizontalBox::Slot()
			   .AutoWidth()
			   .Padding(5, 0, 0, 0)
			   [
				   SNew(SCheckBox)
				   .IsChecked(Task.Finished)
				   .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) {
				   	   UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
				   	   TimelineAsset->TasksRegistry[DisplayedTaskID].Finished = (bool)NewState;
				   	   TimelineAsset->MarkPackageDirty();
				   	   STimelineEditor::Singleton->RefreshDisplayedTimeline();
				   })
			   ]
		   ]
	);
	// @formatter:on

	// Fill the SubTasks vertical box, if needed
	for(uint64 SubTask : Task.SubTasks)
		SubTasksVerticalBox->AddSlot()
		                   .AutoHeight()
		[
			SNew(SDropDown<uint64>)
			.UserData(SubTask)
			.IsDropped(UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[SubTask].VisibleInEditor)
			.OnToggled_Lambda([](bool IsDropped, TAttribute<uint64> UserData) { UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[UserData.Get()].VisibleInEditor = IsDropped; })
			.LeftPadding(TimelineEditor::DropDownsLeftPaddings::Tasks)
			.BarContent()[TimelineEditor::ConstructTaskDropDownBarContent(SubTask)]
			[
				SNew(STimelineEditorTaskDisplay, SubTask)
			]
		];

	return Result;
}

TSharedRef<SWidget> STimelineEditorTaskDisplay::ConstructMemberListTextBox()
{
	TArray Members = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[DisplayedTaskID].AssignedProjectMembers;
	FString Text = Members[0].ToString();
	for(int i = 1; i < Members.Num(); ++i)
		Text += ", " + Members[i].ToString();
	return ConstructHorizontalScrollableText(Text);
}

TSharedRef<SWidget> STimelineEditorTaskDisplay::ConstructButtonPad()
{
	// @formatter:off
	return
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SButton)
		.Text(FText::FromName("Edit"))
		.OnClicked_Lambda([this] {
			TimelineEditor::CreatePopupWindow("Task Editor", TimelineEditor::DefaultPopupSizes::TaskEditor, SNew(STimelineEditorTaskEditor, DisplayedTaskID));
			STimelineEditor::Singleton->RefreshDisplayedTimeline();
			return FReply::Handled();
		})
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SButton)
		.Text(FText::FromName("Delete"))
		.OnClicked_Lambda([this] {
			bool Confirmed;
			TimelineEditor::CreatePopupWindow(
				"Confirm Deletion",
				TimelineEditor::DefaultPopupSizes::ConfirmPopup,
				SNew(STimelineEditorConfirmPopup)
				.OnExited_Lambda([&](bool InConfirmed) { Confirmed = InConfirmed; })
			);
			if(Confirmed)
				TimelineEditor::DeleteTask(DisplayedTaskID);
			return FReply::Handled();
		})
	];
	// @formatter:on
}
