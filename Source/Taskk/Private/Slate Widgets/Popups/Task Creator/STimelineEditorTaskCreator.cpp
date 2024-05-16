#include "STimelineEditorTaskCreator.h"

#include "SlateOptMacros.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Name Space/TimelineEditor.h"
#include "Slate Widgets/Main/STimelineEditor.h"
#include "Slate Widgets/Popups/Task Editor/STimelineEditorTaskEditor.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "Widgets/Input/SNumericEntryBox.h"

void STimelineEditorTaskCreator::Construct(FArguments const & InArgs)
{
	FDateTime Now = FDateTime::Now();

	EnteredDay = Now.GetDay();
	EnteredMonth = Now.GetMonth();
	EnteredYear = Now.GetYear();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
#define MAKE_SLOTS(DatePart)\
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(STextBlock).Text(FText::FromName(#DatePart))]\
			+ SHorizontalBox::Slot()[SNew(SNumericEntryBox<uint32>).Value(Now.Get##DatePart()).OnValueChanged_Lambda([this](uint32 NewValue){ Entered##DatePart = NewValue; })]
			MAKE_SLOTS(Day)
			MAKE_SLOTS(Month)
			MAKE_SLOTS(Year)
			+ SHorizontalBox::Slot().AutoWidth()[SNew(SButton).Text(FText::FromName("Create")).OnClicked_Lambda([this]
			{
				CreateTask();
				return FReply::Handled();
			})]
		]
	];
}

void STimelineEditorTaskCreator::CreateTask()
{
	FDateTime TimeStamp = FDateTime(EnteredYear, EnteredMonth, EnteredDay);
	UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
	// Ensure the time stamp is registered
	if(!TimelineAsset->Timeline.Contains(TimeStamp))
		TimelineAsset->Timeline.Add(TimeStamp);
	// Create a default Task
	FTimelineTask Task = FTimelineTask();
	Task.AssignedProjectMembers = TimelineAsset->ProjectMembers;
	Task.Name = "Unnamed Task";
	Task.Description = "";
	Task.SubTasks = {};
	// Register the new task with a fresh ID
	uint64 TaskID = TimelineAsset->LastTaskID;
	TimelineAsset->TasksRegistry.Add(TaskID, Task);
	// Increase the last ID counter
	++TimelineAsset->LastTaskID;
	// Add the task on the timeline
	TimelineAsset->Timeline[TimeStamp].Tasks.Add(TaskID);
	// Create a task editor for the newly created task
	bool CancelledEditing;
	TimelineEditor::CreatePopupWindow(
		"Task Editor",
		TimelineEditor::DefaultPopupSizes::TaskEditor,
		SNew(STimelineEditorTaskEditor, TaskID).OnExited_Lambda([&](bool InCancelledEditing) { CancelledEditing = InCancelledEditing; })
	);
	// If editing has been cancelled, remove the task from both the registry and the day data
	if(CancelledEditing)
	{
		TimelineAsset->TasksRegistry.Remove(TaskID);
		TArray<uint64> & DayTasks = TimelineAsset->Timeline[TimeStamp].Tasks;
		DayTasks.Remove(TaskID);
		// If there's no other task for that day, remove it from the timeline
		if(!DayTasks.Num())
			TimelineAsset->Timeline.Remove(TimeStamp);
	}
	else
	{
		// Reorder the Timeline
		TimelineAsset->Timeline.KeySort([](FDateTime First, FDateTime Second) { return First < Second; });
		STimelineEditor::Singleton->RefreshDisplayedTimeline();
	}
	// Once the task editor has been closed, destroy this popup
	TimelineEditor::DestroyLastPopup();
}
