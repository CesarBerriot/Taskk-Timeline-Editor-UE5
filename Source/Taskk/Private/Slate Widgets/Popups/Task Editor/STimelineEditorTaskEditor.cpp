#include "STimelineEditorTaskEditor.h"

#include "SlateOptMacros.h"
#include "SNameComboBox.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Name Space/TimelineEditor.h"
#include "Slate Widgets/Button Width Data/SButtonWithData.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Notifications/SNotificationList.h"

void STimelineEditorTaskEditor::Construct(FArguments const& InArgs, uint64 TaskID)
{
	EditedTaskID = TaskID;
	FTimelineTask Task = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[EditedTaskID];
	AssignedProjectMembers = Task.AssignedProjectMembers;
	SubTasks = Task.SubTasks;
	OnExited = InArgs._OnExited;
	// @formatter:off
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromName("Name"))]
		+ SVerticalBox::Slot().AutoHeight()[SAssignNew(NameTextBox, SEditableTextBox).Text(FText::FromName(Task.Name))]
		+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromName("Members"))]
		+ SVerticalBox::Slot().AutoHeight()[ConstructMemberListEditor()]
		+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromName("Description"))]
		+ SVerticalBox::Slot()
		.FillHeight(1)
		[
			// SNew(SBox)
			// .MinDesiredHeight(200)
			// [
				SAssignNew(DescriptionTextBox, SMultiLineEditableTextBox).Text(FText::FromString(Task.Description))
			// ]
		]
		+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromName("SubTasks"))]
		+ SVerticalBox::Slot().AutoHeight()[ConstructSubTaskListEditor()]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 19, 0, 1)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.Text(FText::FromName("Ok"))
			.OnClicked_Lambda([this] { SaveAndExit(); return FReply::Handled(); })
		]
	];
	// @formatter:on
}

TSharedRef<SWidget> STimelineEditorTaskEditor::ConstructMemberListEditor()
{
	// Refresh combo box options
	TArray<TSharedPtr<FName>>& Options = ProjectMemberSelectionComboBoxOptions;
	Options.Empty();
	for (FName Member : UTimelineEditorToolWidget::Singleton->SelectedTimeline->ProjectMembers)
		Options.Add(MakeShareable(new FName(Member)));

	// Construct layout
	// @formatter:off
	TSharedRef<SWidget> Result =
	SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SAssignNew(ProjectMemberSelectionComboBox, SNameComboBox)
			.OptionsSource(&Options)
			.InitiallySelectedItem(Options.Num() ? Options[0] : nullptr)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(FText::FromName("Add"))
			.OnClicked_Lambda([this] { AddSelectedProjectMember(); return FReply::Handled(); })
		]
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(12, 0, 0, 0)
	[
		SAssignNew(ProjectMembersVerticalBox, SVerticalBox)
	];
	// @formatter:on

	RefreshProjectMembersVerticalBox();

	return Result;
}

TSharedRef<SWidget> STimelineEditorTaskEditor::ConstructSubTaskListEditor()
{
	// @formatter:off
	TSharedRef<SWidget> Result =
	SNew(SVerticalBox)
	+ SVerticalBox::Slot().AutoHeight()[SAssignNew(SubTasksVerticalBox, SVerticalBox)]
	+ SVerticalBox::Slot().AutoHeight()
	[
		SNew(SButtonWithData<uint64>, EditedTaskID)
		.Text(FText::FromName("Create"))
		.OnClicked_Lambda([this](uint64 TaskID) {
			UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
			FTimelineTask & Task = TimelineAsset->TasksRegistry[TaskID];
			// Create a default SubTask
			FTimelineTask SubTask = FTimelineTask();
			SubTask.AssignedProjectMembers = Task.AssignedProjectMembers;
			SubTask.Name = "Unnamed SubTask";
			SubTask.Description = "";
			SubTask.SubTasks = {};
			// Register the new task with a fresh ID
			uint64 SubTaskID = TimelineAsset->LastTaskID;
			TimelineAsset->TasksRegistry.Add(SubTaskID, SubTask);
			// Increase the last ID counter
			++TimelineAsset->LastTaskID;
			// Add the SubTask to the SubTask list
			SubTasks.Add(SubTaskID);
			// Create a task editor for the newly created SubTask
			bool CancelledEditing;
			TimelineEditor::CreatePopupWindow(
				"SubTask Editor",
				TimelineEditor::DefaultPopupSizes::TaskEditor,
				SNew(STimelineEditorTaskEditor, SubTaskID).OnExited_Lambda([&](bool InCancelledEditing) { CancelledEditing = InCancelledEditing; })
			);
			// If editing has been cancelled, remove it from both the SubTask list and the Registry
			if(CancelledEditing)
			{
				SubTasks.Remove(SubTaskID);
				TimelineAsset->TasksRegistry.Remove(SubTaskID);
			}

			RefreshSubTasksVerticalBox();
		})
	];
	// @formatter:on
	RefreshSubTasksVerticalBox();
	return Result;
}

void STimelineEditorTaskEditor::RefreshProjectMembersVerticalBox()
{
	// @formatter:off
	ProjectMembersVerticalBox->ClearChildren();
	for(FName Member : AssignedProjectMembers)
		ProjectMembersVerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()[SNew(STextBlock).Text(FText::FromName(Member))]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButtonWithData<FName>, Member)
				.Text(FText::FromName("Delete"))
				.OnClicked_Lambda([this](FName Name){ checkf(AssignedProjectMembers.Contains(Name), L"%ls", *Name.ToString()) AssignedProjectMembers.Remove(Name); RefreshProjectMembersVerticalBox(); })
			]
		];
	// @formatter:on
}

void STimelineEditorTaskEditor::RefreshSubTasksVerticalBox()
{
	SubTasksVerticalBox->ClearChildren();
	for (uint64 SubTaskID : SubTasks)
	{
		SubTasksVerticalBox->AddSlot()
		                   .AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromName(UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[SubTaskID].Name))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButtonWithData<uint64>, SubTaskID)
				.Text(FText::FromName("Edit"))
				.OnClicked_Lambda([this](uint64 SubTaskID)
				                                        {
					                                        TimelineEditor::CreatePopupWindow("SubTask Editor", TimelineEditor::DefaultPopupSizes::TaskEditor, SNew(STimelineEditorTaskEditor, SubTaskID));
					                                        RefreshSubTasksVerticalBox();
				                                        })
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButtonWithData<uint64>, SubTaskID)
				.Text(FText::FromName("Delete"))
				.OnClicked_Lambda([this](uint64 SubTaskID)
				                                        {
					                                        TimelineEditor::DeleteSubTask(SubTaskID);
					                                        SubTasks.Remove(SubTaskID);
					                                        RefreshSubTasksVerticalBox();
				                                        })
			]
		];
	}
}

void STimelineEditorTaskEditor::AddSelectedProjectMember()
{
	if (!ProjectMemberSelectionComboBox->GetSelectedItem())
	{
		FNotificationInfo Info = FNotificationInfo(FText::FromString("Error : Invalid Selection"));
		FSlateNotificationManager::Get().AddNotification(Info);
		return;
	}

	FName SelectedMemberName = *ProjectMemberSelectionComboBox->GetSelectedItem();

	if (AssignedProjectMembers.Contains(SelectedMemberName))
	{
		FNotificationInfo Info = FNotificationInfo(FText::FromString("Error : This member is already assigned to the task"));
		FSlateNotificationManager::Get().AddNotification(Info);
		return;
	}

	AssignedProjectMembers.Add(SelectedMemberName);
	RefreshProjectMembersVerticalBox();
}

void STimelineEditorTaskEditor::SaveAndExit()
{
	FName Name = FName(NameTextBox->GetText().ToString());
	FString Description = DescriptionTextBox->GetText().ToString();

	FNotificationInfo NotificationInfo = FNotificationInfo(FText());
	if (Name == NAME_None)
		NotificationInfo.Text = FText::FromString("Error : Empty Name");
	else if (!AssignedProjectMembers.Num())
		NotificationInfo.Text = FText::FromString("Error : No Assigned Project Members");
	else
	{
		CancelledEditing = false;
		NotificationInfo.Text = FText::FromString("Task Saved");
		FTimelineTask& Task = UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[EditedTaskID];
		Task.Name = Name;
		Task.Description = Description;
		Task.AssignedProjectMembers = AssignedProjectMembers;
		Task.SubTasks = SubTasks;
		TimelineEditor::DestroyLastPopup();
	}
	FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	UTimelineEditorToolWidget::Singleton->SelectedTimeline->MarkPackageDirty();
}
