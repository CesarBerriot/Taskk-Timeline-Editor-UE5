#pragma once

namespace TimelineEditor
{
	inline TArray<TSharedPtr<SWindow>> PopupWindowChain = {};
	void CreatePopupWindow(FName Name, FVector2D WindowSize, TSharedRef<SWidget> Content);
	void DestroyLastPopup();
	void DestroyAllPopups();
	void DeleteTask(uint64 ID, bool CleanDays = true);
	FORCEINLINE void DeleteSubTask(uint64 ID) { DeleteTask(ID, false); }
	TSharedRef<SWidget> ConstructDayDropDownBarContent(FDateTime TimeStamp);
	TSharedRef<SWidget> ConstructTaskDropDownBarContent(uint64 TaskID);
	/// @param ProgressBarPercent 0->1
	TSharedRef<SWidget> ConstructTimelineDropDownBarContent(FString Text, float ProgressBarPercent);
	/// @return 0->1
	float ComputeDayCompletionPercentage(FDateTime TimeStamp);
	/// @return 0->1
	float ComputeTaskCompletionPercentage(uint64 TaskID);
	namespace DefaultPopupSizes
	{
		inline FVector2D ProjectMembersEditor = FVector2D(450, 600);
		inline FVector2D TaskCreator = FVector2D(600, 100);
		inline FVector2D TaskEditor = FVector2D(500, 300);
		inline FVector2D ConfirmPopup = FVector2D(500, 50);
	}
	namespace DropDownsLeftPaddings
	{
		enum Type
		{
			Days  = 20,
			Tasks = 10
		};
	}
};
