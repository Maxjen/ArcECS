#include "ActorComponent/ArcStageExecutorComponent.h"
#include "ArcECSSubsystem.h"
#include "Actor/ArcScheduleExecutor.h"

UArcStageExecutorComponent::UArcStageExecutorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UArcStageExecutorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AArcScheduleExecutor* Executor = Cast<AArcScheduleExecutor>(GetOwner());
	UArcECSSubsystem* ECSSubsystem = UArcECSSubsystem::Get(this);
	if (Executor && ECSSubsystem)
	{
		FArcUniverse& Universe = ECSSubsystem->GetUniverse();
		const FArcSchedule& TickSchedule = Executor->GetTickSchedule();
		Universe.ExecuteCommands();
		for (int32 i = FirstStageIndex; i <= LastStageIndex; ++i)
		{
			TickSchedule.ExecuteStage(Universe, i);
		}
	}
}

void UArcStageExecutorComponent::SetStageRange(int32 InFirstStageIndex, int32 InLastStageIndex)
{
	FirstStageIndex = InFirstStageIndex;
	LastStageIndex = InLastStageIndex;
}
