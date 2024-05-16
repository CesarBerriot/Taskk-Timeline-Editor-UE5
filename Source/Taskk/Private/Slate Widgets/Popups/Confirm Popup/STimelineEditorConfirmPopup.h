#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class TASKK_API STimelineEditorConfirmPopup : public SCompoundWidget
{
public:
	typedef TDelegate<void(bool Confirmed)> FOnExited;
	
	SLATE_BEGIN_ARGS(STimelineEditorConfirmPopup)
		: _Text(FText::FromName("Please confirm that action.")) {}
	SLATE_ARGUMENT(FText, Text)
	SLATE_EVENT(FOnExited, OnExited)
	SLATE_END_ARGS()

	virtual ~STimelineEditorConfirmPopup() override;

	void Construct(FArguments const & InArgs);
private:
	FReply Cancel();
	FReply Confirm();
	FOnExited OnExited;
	bool Confirmed = false;
};
