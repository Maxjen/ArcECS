// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcSystemInternal.h"

struct ARCECS_API FArcScheduleStage
{
    FName StageName;
    TArray<TSharedPtr<FArcSystemInternalBase>> Systems;
    
	static const FName DefaultStage;
	
	static const FName PrePhysicsStage;
	static const FName DuringPhysicsStage;
	static const FName PostPhysicsStage;
	static const FName PostUpdateWorkStage;
	static const FName LastDemotableStage;
};

class ARCECS_API FArcSchedule
{
private:

    TArray<FArcScheduleStage> Stages;

public:

    FArcSchedule() = default;

    void AddStage(const FName& StageName, const TArray<TSharedPtr<FArcSystemInternalBase>>& InSystems);

    void Execute(class FArcUniverse& Universe) const;

    int32 GetStageIndex(const FName& StageName) const;
    void ExecuteStage(class FArcUniverse& Universe, int32 StageIndex) const;

    int32 GetNumStages() const;
};
