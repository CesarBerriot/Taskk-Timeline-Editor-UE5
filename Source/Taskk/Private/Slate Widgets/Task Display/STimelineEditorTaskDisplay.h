#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class TASKK_API STimelineEditorTaskDisplay : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimelineEditorTaskDisplay) {}
	SLATE_END_ARGS()

	void Construct(FArguments const & InArgs, uint64 TaskID);
private:
	FORCEINLINE TSharedRef<SWidget> ConstructMemberNameTextBox(FName Name) { return SNew(STextBlock).Text(FText::FromName(Name)).Font(FSlateFontInfo(FPaths::EngineContentDir() / L"Slate/Fonts/Roboto-Bold.ttf", 12)); }
	FORCEINLINE TSharedRef<SWidget> ConstructHorizontalScrollableText(FString Text) { return ConstructHorizontalScrollableWidget(SNew(STextBlock).Text(FText::FromString(Text))); }
	TSharedRef<SWidget> ConstructHorizontalScrollableWidget(TSharedRef<SWidget> Content);
	TSharedRef<SWidget> ConstructBottomSection();
	TSharedRef<SWidget> ConstructMemberListTextBox();
	TSharedRef<SWidget> ConstructButtonPad();
	uint64 DisplayedTaskID;
};
