#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimelineEditorToolWidget.generated.h"

UCLASS()
class TASKK_API UTimelineEditorToolWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// @formatter:off
	UPROPERTY(EditAnywhere, Transient, Category = "Taskk") class UTimelineAsset * SelectedTimeline;
	// @formatter:on
	static inline ThisClass * Singleton = nullptr;
private:
	typedef FSimpleDelegate FOnSelectedTimelineChanged;
	typedef FSimpleDelegate FOnWidgetDestructed;
	FOnSelectedTimelineChanged OnSelectedTimelineChanged = {};
	FOnWidgetDestructed OnWidgetDestructed = {};
public:
	TSharedRef<SWidget> CreateTimelineSelectorWidget();
private:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeDestruct() override;
};
