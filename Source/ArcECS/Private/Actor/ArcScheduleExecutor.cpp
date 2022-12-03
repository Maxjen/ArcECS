#include "Actor/ArcScheduleExecutor.h"
#include "ArcScheduleBuilder.h"
#include "ActorComponent/ArcStageExecutorComponent.h"

AArcScheduleExecutor::AArcScheduleExecutor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

AArcScheduleExecutor* AArcScheduleExecutor::SpawnScheduleExecutor(UWorld& World, FArcScheduleBuilder& TickScheduleBuilder)
{
	AArcScheduleExecutor* Executor = World.SpawnActor<AArcScheduleExecutor>(AArcScheduleExecutor::StaticClass(), FTransform::Identity);
	Executor->TickSchedule = TickScheduleBuilder.BuildSchedule();

	const FArcSchedule& TickSchedule = Executor->TickSchedule;
	
	int32 PreviousStageIndex = 0;
	auto tryAddExecutorComponent = [&](const FName StageName, ETickingGroup TickGroup, bool bIsLastStage = false)
	{
		if (const int32 StageIndex = TickSchedule.GetStageIndex(StageName))
		{
			UArcStageExecutorComponent* StageExecutor = NewObject<UArcStageExecutorComponent>(Executor);
			StageExecutor->SetStageRange(PreviousStageIndex, bIsLastStage ? TickSchedule.GetNumStages() - 1 : StageIndex);
			StageExecutor->SetTickGroup(TickGroup);
			StageExecutor->RegisterComponent();
			Executor->StageExecutors.Add(StageExecutor);
			PreviousStageIndex = StageIndex + 1;
		}
	};
	tryAddExecutorComponent(FArcScheduleStage::PrePhysicsStage, ETickingGroup::TG_PrePhysics);
	tryAddExecutorComponent(FArcScheduleStage::DuringPhysicsStage, ETickingGroup::TG_DuringPhysics);
	tryAddExecutorComponent(FArcScheduleStage::PostPhysicsStage, ETickingGroup::TG_PostPhysics);
	tryAddExecutorComponent(FArcScheduleStage::PostUpdateWorkStage, ETickingGroup::TG_PostUpdateWork);
	tryAddExecutorComponent(FArcScheduleStage::LastDemotableStage, ETickingGroup::TG_LastDemotable, true);

	return Executor;
}
