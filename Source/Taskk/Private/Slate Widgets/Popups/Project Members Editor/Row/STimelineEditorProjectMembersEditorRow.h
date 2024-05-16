// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class TASKK_API STimelineEditorProjectMembersEditorRow : public SCompoundWidget
{
public:
	typedef TDelegate<void()> FOnDeleted;
	typedef TDelegate<void()> FOnRenamed;

	SLATE_BEGIN_ARGS(STimelineEditorProjectMembersEditorRow) {}
		SLATE_EVENT(FOnDeleted, OnDeleted)
		SLATE_EVENT(FOnRenamed, OnRenamed)
	SLATE_END_ARGS()

	void Construct(FArguments const & InArgs, FName MemberName);
private:
	TSharedRef<SWidget> ConstructNameDisplayHorizontalBox();
	TSharedRef<SWidget> ConstructNameEditHorizontalBox();
	FORCEINLINE void SwitchToDisplayUI() { DynamicUIContainer->SetContent(ConstructNameDisplayHorizontalBox()); }
	FORCEINLINE void SwitchToEditUI() { DynamicUIContainer->SetContent(ConstructNameEditHorizontalBox()); }
	void Delete();
	bool Rename(FName NewName);
	FName ProjectMember;
	FOnDeleted OnDeleted;
	FOnRenamed OnRenamed;
	TSharedPtr<SBox> DynamicUIContainer;
};
