#include "STimelineEditor.h"

#include "SlateOptMacros.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UMG Widgets/TimelineEditorToolWidget.h"
#include "Name Space/TimelineEditor.h"
#include "Slate Widgets/Drop Down/SDropDown.h"
#include "Slate Widgets/Popups/Confirm Popup/STimelineEditorConfirmPopup.h"
#include "Slate Widgets/Popups/Project Members Editor/STimelineEditorProjectMembersEditor.h"
#include "Slate Widgets/Popups/Task Creator/STimelineEditorTaskCreator.h"
#include "Slate Widgets/Task Display/STimelineEditorTaskDisplay.h"
#include "Timeline Data Asset/TimelineAsset.h"
#include "UEUtils/AssetActions/AssetActions.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SNotificationList.h"

void STimelineEditor::Construct(FArguments const & InArgs)
{
	check(!Singleton)
	Singleton = SharedThis(this);
	// @formatter:off
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()[UTimelineEditorToolWidget::Singleton->CreateTimelineSelectorWidget()].AutoHeight()
		+SVerticalBox::Slot()
		[
			SAssignNew(MainVerticalBox, SVerticalBox)
			+SVerticalBox::Slot().AutoHeight()[ConstructButtonPad()]
			+SVerticalBox::Slot()[SAssignNew(DaysScrollBox, SScrollBox).Orientation(Orient_Vertical)]
		]
	];
	// @formatter:on
	RefreshDisplayedTimeline();
}

STimelineEditor::~STimelineEditor()
{
	check(Singleton.Get() == this)
	Singleton.Reset();
	TimelineEditor::DestroyAllPopups();
}

void STimelineEditor::RefreshDisplayedTimeline()
{
	UE_LOG(LogTemp, Display, L"Timeline Editor : Refreshed displayed timeline")
	auto * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
	UE_LOG(LogTemp, Display, L"Selected Timeline : %ls", *(TimelineAsset ? TimelineAsset->GetFName() : NAME_None).ToString())
	DaysScrollBox->ClearChildren();
	if(!TimelineAsset)
		return;
	// For each day in the timeline
	for(TTuple<FDateTime, FTimelineDayData> Pair : TimelineAsset->Timeline)
	{
		// Create day dropdown
		TSharedPtr<SVerticalBox> TasksVerticalBox;
		DaysScrollBox->AddSlot()
		[
			SNew(SDropDown<FDateTime>)
			.UserData(Pair.Key)
			.IsDropped(UTimelineEditorToolWidget::Singleton->SelectedTimeline->Timeline[Pair.Key].VisibleInEditor)
			.OnToggled_Lambda([](bool IsDropped, TAttribute<FDateTime> UserData) { UTimelineEditorToolWidget::Singleton->SelectedTimeline->Timeline[UserData.Get()].VisibleInEditor = IsDropped; })
			.BarContent()[TimelineEditor::ConstructDayDropDownBarContent(Pair.Key)]
			.LeftPadding(TimelineEditor::DropDownsLeftPaddings::Days)
			[
				// VerticalBox content
				SAssignNew(TasksVerticalBox, SVerticalBox)
			]
		];
		// Add tasks to VerticalBox
		for(uint64 TaskID : Pair.Value.Tasks)
		{
			// @formatter:off
			FTimelineTask Task = TimelineAsset->TasksRegistry[TaskID];
			TasksVerticalBox->AddSlot()
			.AutoHeight()
			[
				SNew(SDropDown<uint64>)
				.UserData(TaskID)
				.IsDropped(UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[TaskID].VisibleInEditor)
				.OnToggled_Lambda([](bool IsDropped, TAttribute<uint64> UserData){ UTimelineEditorToolWidget::Singleton->SelectedTimeline->TasksRegistry[UserData.Get()].VisibleInEditor = IsDropped; })
				.LeftPadding(TimelineEditor::DropDownsLeftPaddings::Tasks)
				.BarContent()[TimelineEditor::ConstructTaskDropDownBarContent(TaskID)]
				[
					SNew(STimelineEditorTaskDisplay, TaskID)
				]
			];
			// @formatter:on
		}
	}
}

void STimelineEditor::OnWrapperDestructedCallback()
{
	UE_LOG(LogTemp, Warning, L"ta pute de mere")
	// Safe Confirm Popup
	UTimelineAsset * SelectedTimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
	if(SelectedTimelineAsset && SelectedTimelineAsset->GetPackage()->IsDirty())
	{
		bool Confirmed;
		TimelineEditor::CreatePopupWindow(
			"Save Asset",
			TimelineEditor::DefaultPopupSizes::ConfirmPopup,
			SNew(STimelineEditorConfirmPopup)
			.Text(FText::FromString(FString::Printf(L"Save Asset '%ls' ?", *SelectedTimelineAsset->GetName())))
			.OnExited_Lambda([&Confirmed](bool InConfirmed) { Confirmed = InConfirmed; })
		);
		if(Confirmed)
			check(UEUtils::AssetActions::SaveAsset(SelectedTimelineAsset) == UEUtils::AssetActions::ESaveError::Success);
	}
}

void STimelineEditor::Tick(FGeometry const & AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	// Enable/Disable **MainVerticalBox**
	bool NewState = (bool)UTimelineEditorToolWidget::Singleton->SelectedTimeline;
	if(MainVerticalBox->IsEnabled() != NewState)
		RefreshDisplayedTimeline();
	MainVerticalBox->SetEnabled(NewState);
}

TSharedRef<SWidget> STimelineEditor::ConstructButtonPad()
{
	namespace Sizes = TimelineEditor::DefaultPopupSizes;
	// @formatter:off
	return
	SAssignNew(ButtonPad, SHorizontalBox)
	+ SHorizontalBox::Slot().AutoWidth()[ConstructButtonPadSaveButton()]
	+ SHorizontalBox::Slot().AutoWidth()[ConstructButtonPadButton("Edit Project Members", [this] {
		TimelineEditor::CreatePopupWindow("Project Members Editor", Sizes::ProjectMembersEditor, SNew(STimelineEditorProjectMembersEditor)); RefreshDisplayedTimeline(); return FReply::Handled(); })]
	+ SHorizontalBox::Slot().AutoWidth()[ConstructButtonPadButton("Create Task", [this]	{ TryCreateNewTask(); return FReply::Handled(); })]
	// @todo Remove, or make it cleaner. Whatever just do smth.
	+ SHorizontalBox::Slot().AutoWidth()[ConstructButtonPadButton("(Debug) Remove Everything", [this] {
		UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
		TimelineAsset->TasksRegistry.Empty();
		TimelineAsset->Timeline.Empty();
		TimelineAsset->LastTaskID = 0;
		RefreshDisplayedTimeline();
		return FReply::Handled();
	})];
	// @formatter:on
}

TSharedRef<SWidget> STimelineEditor::ConstructButtonPadButton(FString Text, TFunction<FReply()> Callback)
{
	return
	SNew(SButton)
	.Text(FText::FromString(Text))
	.OnClicked_Lambda(Callback);
}

TSharedRef<SWidget> STimelineEditor::ConstructButtonPadSaveButton()
{
	// @formatter:off
	return
	SNew(SButton)
	.Text_Lambda([] {
		UTimelineAsset * TimelineAsset = UTimelineEditorToolWidget::Singleton->SelectedTimeline;
		bool IsDirty = TimelineAsset ? TimelineAsset->GetPackage()->IsDirty() : false;
		FString Result = "Save";
		if(IsDirty)
			Result += '*';
		return FText::FromString(Result);
	})
	.OnClicked_Lambda([this] { SaveTimeline(); return FReply::Handled(); });
	// @formatter:on
}

void STimelineEditor::SaveTimeline()
{
	typedef UEUtils::AssetActions::ESaveError ESaveError;
	ESaveError Error = UEUtils::AssetActions::SaveAsset(UTimelineEditorToolWidget::Singleton->SelectedTimeline);

	FNotificationInfo NotificationInfo = FNotificationInfo(FText());
	NotificationInfo.Text = FText::FromString(Error == ESaveError::Success ? "Timeline Saved" : "Timeline Save Failed");
	if(Error != ESaveError::Success)
		NotificationInfo.SubText = FText::FromString(FString::Printf(L"Error Code : %i", Error));

	FSlateNotificationManager::Get().AddNotification(NotificationInfo);
}

void STimelineEditor::TryCreateNewTask()
{
	if(!UTimelineEditorToolWidget::Singleton->SelectedTimeline->ProjectMembers.Num())
	{
		FNotificationInfo NotificationInfo = FNotificationInfo(FText::FromString("Error : No Project Members"));
		NotificationInfo.SubText = FText::FromString("Registered Project Members are required to create tasks.");
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return;
	}
	TimelineEditor::CreatePopupWindow("Task Creation", TimelineEditor::DefaultPopupSizes::TaskCreator, SNew(STimelineEditorTaskCreator));
	RefreshDisplayedTimeline();
}
