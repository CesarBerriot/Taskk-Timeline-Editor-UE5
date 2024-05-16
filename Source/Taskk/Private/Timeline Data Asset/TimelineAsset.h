#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TimelineAsset.generated.h"

USTRUCT()
struct FTimelineTask
{
	GENERATED_BODY()
public:
	// @formatter:off
	UPROPERTY(VisibleAnywhere, Category = "Taskk") TArray<FName> AssignedProjectMembers;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") FName Name;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") FString Description;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") bool VisibleInEditor;
	/// Whether this task has been finished. Ignored if any subtasks are registered.
	UPROPERTY(VisibleAnywhere, Category = "Taskk") bool Finished;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") TArray<uint64> SubTasks;
	// @formatter:on
};

USTRUCT()
struct FTimelineDayData
{
	GENERATED_BODY()
public:
	// @formatter:off
	UPROPERTY(VisibleAnywhere, Category = "Taskk") TArray<uint64> Tasks;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") bool VisibleInEditor;
	// @formatter:on
};

UCLASS()
class TASKK_API UTimelineAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// @formatter:off
	UPROPERTY(VisibleAnywhere, Category = "Taskk") TArray<FName> ProjectMembers;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") uint64 LastTaskID;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") TMap<uint64, FTimelineTask> TasksRegistry;
	UPROPERTY(VisibleAnywhere, Category = "Taskk") TMap<FDateTime, FTimelineDayData> Timeline;
	// @formatter:on
};
