#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SScrollBox;

class TASKK_API STimelineEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimelineEditor) {}
	SLATE_END_ARGS()
	
	void Construct(FArguments const & InArgs);
	virtual ~STimelineEditor() override;

	static inline TSharedPtr<STimelineEditor> Singleton = {};
	/// Refreshes all days and tasks displayed
	void RefreshDisplayedTimeline();
	/// Called when the blueprint wrapper UUserWidget gets destructed
	void OnWrapperDestructedCallback();
private:
	virtual void Tick(FGeometry const & AllottedGeometry, double InCurrentTime, float InDeltaTime) override;
	TSharedRef<SWidget> ConstructButtonPad();
	TSharedRef<SWidget> ConstructButtonPadButton(FString Text, TFunction<FReply()> Callback);
	TSharedRef<SWidget> ConstructButtonPadSaveButton();
	/// Callback of 'Save' button
	void SaveTimeline();
	/// Callback of 'Create Task' button
	void TryCreateNewTask();
	TSharedPtr<SHorizontalBox> ButtonPad;
	TSharedPtr<SScrollBox> DaysScrollBox;
	/// Contains everything but the Timeline Selector
	TSharedPtr<SVerticalBox> MainVerticalBox;
};
