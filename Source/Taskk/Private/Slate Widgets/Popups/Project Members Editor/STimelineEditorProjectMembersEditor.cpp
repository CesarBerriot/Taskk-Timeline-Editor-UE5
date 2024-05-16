#include "STimelineEditorProjectMembersEditor.h"

#include "SlateOptMacros.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Row/STimelineEditorProjectMembersEditorRow.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SNotificationList.h"

void STimelineEditorProjectMembersEditor::Construct(FArguments const & InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.Padding(7)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()[ConstructTitleText()].AutoHeight()
			+ SVerticalBox::Slot()[ConstructMemberCreationSection()].VAlign(VAlign_Top).AutoHeight()
			+ SVerticalBox::Slot()[SAssignNew(ScrollBox, SScrollBox)]
		]
	];
	RefreshScrollBox();
}

TSharedRef<SWidget> STimelineEditorProjectMembersEditor::ConstructTitleText()
{
	return
	SNew(SBox)
	.MinDesiredHeight(30)
	[
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFit)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Project Members"))
		]
	];
}

TSharedRef<SWidget> STimelineEditorProjectMembersEditor::ConstructMemberCreationSection()
{
	// @formatter:off
	return
	SNew(SHorizontalBox)
	+SHorizontalBox::Slot()
	.HAlign(HAlign_Fill)
	[
		SAssignNew(NewMemberNameTextBox, SEditableTextBox)
		.HintText(FText::FromString("Member Name"))
	]
	+SHorizontalBox::Slot()
	.HAlign(HAlign_Right)
	[
		SNew(SButton)
		.Text(FText::FromName("Add"))
		.OnClicked_Lambda([this] {
			UTimelineAsset * Timeline = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
			FName NewMemberName = FName(NewMemberNameTextBox->GetText().ToString());
			// Check for errors
			bool Errored = true;
			FNotificationInfo NotificationInfo = FNotificationInfo(FText());
			if(NewMemberName == NAME_None)
				NotificationInfo.Text = FText::FromString("Error : New Member Name can't be empty");
			else if(Timeline->ProjectMembers.Contains(NewMemberName))
				NotificationInfo.Text = FText::FromString("Error : Member Already Exists");
			else
				Errored = false;
			// Create the error notification if needed, else just create the member
			if(Errored)
				FSlateNotificationManager::Get().AddNotification(NotificationInfo);
			else
			{
				NewMemberNameTextBox->SetText({});
				Timeline->ProjectMembers.Add(NewMemberName);
				RefreshScrollBox();
			}
			return FReply::Handled();
		})
	];
	// @formatter:off
}

void STimelineEditorProjectMembersEditor::RefreshScrollBox()
{
	ScrollBox->ClearChildren();
	for(FName ProjectMember : UTimelineEditorToolWidget::Singleton->SelectedTimeline->ProjectMembers)
	{
		ScrollBox->AddSlot()
		[
			SNew(STimelineEditorProjectMembersEditorRow, ProjectMember)
			.OnDeleted_Raw(this, &STimelineEditorProjectMembersEditor::RefreshScrollBox)
		];
	}
}
