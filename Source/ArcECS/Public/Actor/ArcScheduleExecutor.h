#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArcSchedule.h"
#include "ArcScheduleExecutor.generated.h"

UCLASS()
class ARCECS_API AArcScheduleExecutor : public AActor
{
	GENERATED_BODY()

private:

	FArcSchedule TickSchedule;

	UPROPERTY()
	TArray<class UArcStageExecutorComponent*> StageExecutors;

public:

	AArcScheduleExecutor();

	static AArcScheduleExecutor* SpawnScheduleExecutor(UWorld& World, struct FArcScheduleBuilder& TickScheduleBuilder);

	const FArcSchedule& GetTickSchedule() { return TickSchedule; }
};
