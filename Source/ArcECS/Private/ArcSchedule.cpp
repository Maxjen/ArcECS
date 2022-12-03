// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcSchedule.h"
#include "ArcUniverse.h"

const FName FArcScheduleStage::DefaultStage = TEXT("DefaultStage");
const FName FArcScheduleStage::PrePhysicsStage = TEXT("UE_PrePhysics");
const FName FArcScheduleStage::DuringPhysicsStage = TEXT("UE_DuringPhysics");
const FName FArcScheduleStage::PostPhysicsStage = TEXT("UE_PostPhysics");
const FName FArcScheduleStage::PostUpdateWorkStage = TEXT("UE_PostUpdateWork");
const FName FArcScheduleStage::LastDemotableStage = TEXT("UE_LastDemotable");

void FArcSchedule::AddStage(const FName& StageName, const TArray<TSharedPtr<FArcSystemInternalBase>>& InSystems)
{
    FArcScheduleStage& Stage = Stages.Emplace_GetRef();
    Stage.StageName = StageName;
    Stage.Systems = InSystems;
}

void FArcSchedule::Execute(FArcUniverse& Universe) const
{
    Universe.ExecuteCommands();
    for (int32 i = 0; i < Stages.Num(); ++i)
    {
        ExecuteStage(Universe, i);
    }
}

int32 FArcSchedule::GetStageIndex(const FName& StageName) const
{
    for (int32 i = 0; i < Stages.Num(); ++i)
    {
        if (Stages[i].StageName == StageName) { return i; }
    }
    return INDEX_NONE;
}

void FArcSchedule::ExecuteStage(FArcUniverse& Universe, int32 StageIndex) const
{
    if (!Stages.IsValidIndex(StageIndex)) { return; }
    
    for (const auto& System : Stages[StageIndex].Systems)
    {
        System->Execute(Universe);
        Universe.ExecuteCommands();
    }
}

int32 FArcSchedule::GetNumStages() const
{
    return Stages.Num();
}
