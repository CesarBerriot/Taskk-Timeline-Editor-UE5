#include "STimelineEditorProjectMembersEditorRow.h"

#include "SlateOptMacros.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Name Space/TimelineEditor.h"
#include "Slate Widgets/Button Width Data/SButtonWithData.h"
#include "Slate Widgets/Main/STimelineEditor.h"
#include "Slate Widgets/Popups/Confirm Popup/STimelineEditorConfirmPopup.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "Widgets/Notifications/SNotificationList.h"

void STimelineEditorProjectMembersEditorRow::Construct(FArguments const & InArgs, FName MemberName)
{
	ProjectMember = MemberName;
	OnDeleted = InArgs._OnDeleted;
	OnRenamed = InArgs._OnRenamed;
	// @formatter:off
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()[SAssignNew(DynamicUIContainer, SBox)]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.AutoWidth()
		[
			SNew(SButton)
			.Text(FText::FromName("Delete"))
			.OnClicked_Lambda([this]{ Delete(); return FReply::Handled(); })
		]
	];
	// @formatter:on
	SwitchToDisplayUI();
}

TSharedRef<SWidget> STimelineEditorProjectMembersEditorRow::ConstructNameDisplayHorizontalBox()
{
	// @formatter:off
	return
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	[
		SNew(STextBlock)
		.Text(FText::FromName(ProjectMember))
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SButton)
		.Text(FText::FromName("..."))
		.OnClicked_Lambda([this]{ SwitchToEditUI(); return FReply::Handled(); })
	];
	// @formatter:on
}

TSharedRef<SWidget> STimelineEditorProjectMembersEditorRow::ConstructNameEditHorizontalBox()
{
	TSharedPtr<SEditableTextBox> EditableTextBox;
	// @formatter:off
	return
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	[
		SAssignNew(EditableTextBox, SEditableTextBox)
		.HintText(FText::FromName("New Member Name"))
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SButtonWithData<TSharedPtr<SEditableTextBox>>, EditableTextBox)
		.Text(FText::FromName("Ok"))
		.OnClicked_Lambda([this] (TSharedPtr<SEditableTextBox> Data) {
			if(Rename(FName(Data->GetText().ToString())))
				SwitchToDisplayUI();
		})
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SButton)
		.Text(FText::FromName("Cancel"))
		.OnClicked_Lambda([this]{ SwitchToDisplayUI(); return FReply::Handled(); })
	];
	// @formatter:on
}

void STimelineEditorProjectMembersEditorRow::Delete()
{
	TArray<uint64> OwnedTasks = TArray<uint64>();
	TArray<uint64> SharedTasks = TArray<uint64>();

	UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;

	// Gather owned and shared tasks
	for(TTuple<uint64, FTimelineTask> Pair : TimelineAsset->TasksRegistry)
		if(Pair.Value.AssignedProjectMembers.Contains(ProjectMember))
			(Pair.Value.AssignedProjectMembers.Num() > 1 ? SharedTasks : OwnedTasks).Add(Pair.Key);
	for(uint64 OwnedTask : OwnedTasks)
		UE_LOG(LogTemp, Warning, L"Owned Task : %llu", OwnedTask)
	for(uint64 SharedTask : SharedTasks)
		UE_LOG(LogTemp, Warning, L"Shared Task : %llu", SharedTask)

	// Create the confirm popup if shared/owned tasks were found
	if(OwnedTasks.Num() | SharedTasks.Num())
	{
		// Create Confirm Popup String
		FString ConfirmPopupString = FString();
		ConfirmPopupString += FString::Printf(
			L"Project Member %ls owns %i tasks and shared %i tasks within the timeline.\n"
			"Delete owned tasks and remove '%ls' from shared tasks ?\n\n",
			*ProjectMember.ToString(),
			OwnedTasks.Num(),
			SharedTasks.Num(),
			*ProjectMember.ToString()
		);
		ConfirmPopupString += "Owned Tasks :\n";
		for(uint64 OwnedTask : OwnedTasks)
			ConfirmPopupString += "- " + TimelineAsset->TasksRegistry[OwnedTask].Name.ToString() + '\n';
		ConfirmPopupString += "\nShared Tasks :\n";
		for(uint64 SharedTask : SharedTasks)
			ConfirmPopupString += "- " + TimelineAsset->TasksRegistry[SharedTask].Name.ToString() + '\n';
		// Create Confirm Popup
		bool Confirmed;
		TimelineEditor::CreatePopupWindow(
			"Confirm Member Deletion",
			FVector2D(TimelineEditor::DefaultPopupSizes::ConfirmPopup.X, 500),
			SNew(STimelineEditorConfirmPopup)
			.OnExited_Lambda([&](bool InConfirmed) { Confirmed = InConfirmed; })
			.Text(FText::FromString(ConfirmPopupString))
		);

		// Cancel the operation, if requested
		if(!Confirmed)
			return;

		
	}
	//UTimelineEditorToolWidget::Singleton->SelectedTimeline->ProjectMembers.Remove(ProjectMember);
	//OnDeleted.ExecuteIfBound();
}

bool STimelineEditorProjectMembersEditorRow::Rename(FName NewName)
{
	UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;

	// Check for errors
	bool Errored = true;
	FNotificationInfo NotificationInfo = FNotificationInfo(FText());

	if(NewName == NAME_None)
		NotificationInfo.Text = FText::FromString("Error : New Member Name can't be empty");
	else if(NewName == ProjectMember)
		NotificationInfo.Text = FText::FromString("Error : New Member Name can't be equal to the previous one");
	else if(TimelineAsset->ProjectMembers.Contains(NewName))
		NotificationInfo.Text = FText::FromString("Error : New Member Name can't be equal to another member's");
	else
		Errored = false;

	if(Errored)
	{
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return false;
	}

	// Refresh this widget's displayed name
	FName OldName = ProjectMember;
	ProjectMember = NewName;

	// Apply to project members list
	TimelineAsset->ProjectMembers.Remove(OldName);
	TimelineAsset->ProjectMembers.Add(NewName);

	/// Function that goes through the whole SubTask tree of a task and applies the rename
	TFunction<void(uint64)> ApplyRenameToTask = [&](uint64 TaskID)
	{
		FTimelineTask & Task = TimelineAsset->TasksRegistry[TaskID];
		if(Task.AssignedProjectMembers.Contains(OldName))
		{
			Task.AssignedProjectMembers.Remove(OldName);
			Task.AssignedProjectMembers.Add(NewName);
		}
		for(uint64 SubTaskID : Task.SubTasks)
			ApplyRenameToTask(SubTaskID);
	};

	// Calls **TimelineAsset()** for the whole registry
	for(auto Task : TimelineAsset->TasksRegistry)
		ApplyRenameToTask(Task.Key);

	// Mark **TimelineAsset** Dirty
	TimelineAsset->MarkPackageDirty();

	// Refresh UI
	STimelineEditor::Singleton->RefreshDisplayedTimeline();

	// Return
	OnRenamed.ExecuteIfBound();
	return true;
}
