#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class TASKK_API STimelineEditorTaskCreator : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimelineEditorTaskCreator) {}
	SLATE_END_ARGS()

	void Construct(FArguments const & InArgs);
private:
	void CreateTask();
	int32 EnteredDay;
	int32 EnteredMonth;
	int32 EnteredYear;
};
