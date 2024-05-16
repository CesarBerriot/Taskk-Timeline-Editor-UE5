// Fill out your copyright notice in the Description page of Project Settings.


#include "TimelineEditorToolWidget.h"

#include "ISinglePropertyView.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SinglePropertyView.h"
#include "Slate Widgets/Main/STimelineEditor.h"
#include "UEUtils/Template/Template.h"

/// Used in **UTimelineEditorToolWidget::CreateTimelineSelectorWidget()**
DECLARE_PRIVATE_OFFSET(USinglePropertyView, SinglePropertyViewWidget)

TSharedRef<SWidget> UTimelineEditorToolWidget::CreateTimelineSelectorWidget()
{
	// Create property editor
	USinglePropertyView * SinglePropertyView = WidgetTree->ConstructWidget<USinglePropertyView>();
	SinglePropertyView->SetObject(this);
	SinglePropertyView->SetPropertyName("SelectedTimeline");
	
	// Ensure the slate widget has been constructed
	TSharedRef<SWidget> SlateWidget = SinglePropertyView->TakeWidget();
	
	// Bind OnValueChanged
	FSimpleDelegate Callback = FSimpleDelegate::CreateLambda([this]{OnSelectedTimelineChanged.ExecuteIfBound();});
	/*
	 * @note
	 * I know this is unintended by Epic but there's literally no other way to do it
	 * (excepted a Tick checking the value to see whether it changed but that's awful)
	 * so I really couldn't care less.
	 */
	GET_PRIVATE_MEMBER(USinglePropertyView, SinglePropertyViewWidget, TSharedPtr<ISinglePropertyView>, SinglePropertyView)
	->SetOnPropertyValueChanged(Callback);
	
	// Return it
	return SlateWidget;
}

TSharedRef<SWidget> UTimelineEditorToolWidget::RebuildWidget()
{
	if(Singleton)
		return Singleton->TakeWidget();
	Singleton = this;
	TSharedRef<STimelineEditor> Result = STimelineEditor::Singleton ? STimelineEditor::Singleton.ToSharedRef() : SNew(STimelineEditor);
	OnSelectedTimelineChanged.BindRaw(&Result.Get(), &STimelineEditor::RefreshDisplayedTimeline);
	OnWidgetDestructed.BindRaw(&Result.Get(), &STimelineEditor::OnWrapperDestructedCallback);
	return Result;
}

void UTimelineEditorToolWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Warning, L"" __FUNCTION__)
	OnWidgetDestructed.ExecuteIfBound();
}
