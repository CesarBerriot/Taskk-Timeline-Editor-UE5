#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SScrollBox;

class TASKK_API STimelineEditorProjectMembersEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimelineEditorProjectMembersEditor) {}
	SLATE_END_ARGS()

	void Construct(FArguments const & InArgs);
private:
	TSharedRef<SWidget> ConstructTitleText();
	TSharedRef<SWidget> ConstructMemberCreationSection();
	void RefreshScrollBox();
	TSharedPtr<SScrollBox> ScrollBox;
	TSharedPtr<SEditableTextBox> NewMemberNameTextBox;
};
