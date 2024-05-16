#include "TimelineEditor.h"

#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Slate Widgets/Main/STimelineEditor.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "Widgets/Notifications/SProgressBar.h"

void TimelineEditor::CreatePopupWindow(FName Name, FVector2D WindowSize, TSharedRef<SWidget> Content)
{
	TSharedPtr<SWidget> TimelineEditorWidget = STimelineEditor::Singleton;
	PopupWindowChain.Add({});
	FSlateApplication::Get().AddModalWindow(
		SAssignNew(PopupWindowChain.Last(), SWindow)
				.Title(FText::FromName(Name))
				.ClientSize(WindowSize)
				// @bug Gets ignored
				.ScreenPosition(TimelineEditorWidget->GetCachedGeometry().GetAbsolutePosition())
				.Content()
		[
			Content
		],
		TimelineEditorWidget
	);
	PopupWindowChain.Pop();
}

void TimelineEditor::DestroyLastPopup()
{
	PopupWindowChain.Last()->RequestDestroyWindow();
}

void TimelineEditor::DestroyAllPopups()
{
	while(PopupWindowChain.Num())
		DestroyLastPopup();
}

void TimelineEditor::DeleteTask(uint64 ID, bool CleanDays)
{
	UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;

	// Delete all SubTasks
	while(TimelineAsset->TasksRegistry[ID].SubTasks.Num())
		DeleteTask(TimelineAsset->TasksRegistry[ID].SubTasks[0]);

	/// Function that recursively goes through the whole task tree of a task and removes the given task ID
	TFunction<void(FTimelineTask &, uint64)> CleanTask = [&](FTimelineTask & CleanedTask, uint64 RemovedTaskID)
	{
		CleanedTask.SubTasks.Remove(RemovedTaskID);
		for(uint64 & SubTaskID : CleanedTask.SubTasks)
			CleanTask(TimelineAsset->TasksRegistry[SubTaskID], RemovedTaskID);
	};

	// Clean Registry
	TimelineAsset->TasksRegistry.Remove(ID);

	// Clean all tasks
	TArray<uint64> RegistryKeys;
	TimelineAsset->TasksRegistry.GetKeys(RegistryKeys);
	for(uint64 RegistryKey : RegistryKeys)
		CleanTask(TimelineAsset->TasksRegistry[RegistryKey], ID);

	// Clean days
	if(CleanDays)
	{
		TArray<FDateTime> TimeStampKeys;
		TimelineAsset->Timeline.GetKeys(TimeStampKeys);
		for(FDateTime TimeStampKey : TimeStampKeys)
		{
			TArray<uint64> & DayTasks = TimelineAsset->Timeline[TimeStampKey].Tasks;
			DayTasks.Remove(ID);
			if(!DayTasks.Num())
				TimelineAsset->Timeline.Remove(TimeStampKey);
		}
	}

	// Refresh UI
	STimelineEditor::Singleton->RefreshDisplayedTimeline();

	// Mark **TimelineAsset** Dirty
	TimelineAsset->MarkPackageDirty();
}

TSharedRef<SWidget> TimelineEditor::ConstructDayDropDownBarContent(FDateTime TimeStamp)
{
	TArray Tasks = UTimelineEditorToolWidget::Singleton->SelectedTimeline->Timeline[TimeStamp].Tasks;
	return ConstructTimelineDropDownBarContent(TimeStamp.ToFormattedString(L"%A, %B %e, %Y"), TimelineEditor::ComputeDayCompletionPercentage(TimeStamp));
}

TSharedRef<SWidget> TimelineEditor::ConstructTaskDropDownBarContent(uint64 TaskID)
{
	FTimelineTask Task = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[TaskID];
	return ConstructTimelineDropDownBarContent(Task.Name.ToString(), TimelineEditor::ComputeTaskCompletionPercentage(TaskID));
}

TSharedRef<SWidget> TimelineEditor::ConstructTimelineDropDownBarContent(FString Text, float ProgressBarPercent)
{
	TSharedRef<SHorizontalBox> HorizontalBox =
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	  .AutoWidth()
	  .VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(FText::FromString(Text))
	]
	+ SHorizontalBox::Slot()
	  .Padding(5, 5, 0, 5)
	  .AutoWidth()
	[
		SNew(SBox)
		.MinDesiredWidth(80)
		[
			SNew(SProgressBar)
			.Percent(ProgressBarPercent)
		]
	];
	// @formatter:off
	if(ProgressBarPercent >= 1)
		HorizontalBox->AddSlot()
		.AutoWidth()
		.Padding(5, 0, 0, 0)
		[
			SNew(SImage)
			.Image(FAppStyle::Get().GetBrush("Perforce.CheckedOutByOtherUserOtherBranch"))
		];
	// @formatter:on
	return HorizontalBox;
}

float TimelineEditor::ComputeDayCompletionPercentage(FDateTime TimeStamp)
{
	UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
	TArray Tasks = TimelineAsset->Timeline[TimeStamp].Tasks;

	if(!Tasks.Num())
		return 0;

	float Average = 0;
	for(uint64 TaskID : Tasks)
		Average += ComputeTaskCompletionPercentage(TaskID);
	Average /= Tasks.Num();

	return Average;
}

float TimelineEditor::ComputeTaskCompletionPercentage(uint64 TaskID)
{
	FTimelineTask Task = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[TaskID];

	if(!Task.SubTasks.Num())
		return Task.Finished;

	float Average = 0;
	for(uint64 SubTask : Task.SubTasks)
		Average += ComputeTaskCompletionPercentage(SubTask);
	Average /= Task.SubTasks.Num();

	return Average;
}
