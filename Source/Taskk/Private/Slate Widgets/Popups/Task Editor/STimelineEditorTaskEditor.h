#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class TASKK_API STimelineEditorTaskEditor : public SCompoundWidget
{
public:
	/// Whether the user clicked "Ok" or cancelled the edit by closing the window
	typedef TDelegate<void(bool CancelledEditing)> FOnExited;

	SLATE_BEGIN_ARGS(STimelineEditorTaskEditor) {}
		SLATE_EVENT(FOnExited, OnExited)
	SLATE_END_ARGS()

	FORCEINLINE virtual ~STimelineEditorTaskEditor() override { OnExited.ExecuteIfBound(CancelledEditing); }

	/** Constructs this widget with InArgs */
	void Construct(FArguments const & InArgs, uint64 TaskID);
private:
	TSharedRef<SWidget> ConstructMemberListEditor();
	TSharedRef<SWidget> ConstructSubTaskListEditor();
	void RefreshProjectMembersVerticalBox();
	void RefreshSubTasksVerticalBox();
	void AddSelectedProjectMember();
	/// @note Does **not** save the data asset, just the task itself
	void SaveAndExit();
	uint64 EditedTaskID;
	TArray<FName> AssignedProjectMembers;
	TArray<uint64> SubTasks;
	TSharedPtr<SVerticalBox> ProjectMembersVerticalBox;
	TSharedPtr<class SNameComboBox> ProjectMemberSelectionComboBox;
	TArray<TSharedPtr<FName>> ProjectMemberSelectionComboBoxOptions;
	TSharedPtr<SVerticalBox> SubTasksVerticalBox;
	TSharedPtr<class SEditableTextBox> NameTextBox;
	TSharedPtr<class SMultiLineEditableTextBox> DescriptionTextBox;
	TSharedPtr<class SPinComboBox> SubTasksComboBox;
	FOnExited OnExited;
	/// @see **FOnExited**
	bool CancelledEditing = true;
};
