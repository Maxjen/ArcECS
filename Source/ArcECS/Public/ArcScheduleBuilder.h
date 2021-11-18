// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcSchedule.h"
#include "ArcSystem.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArcECS, Log, All);

struct ARCECS_API FArcScheduleStage
{
public:

	static const FName DefaultStage;

	FName StageLabel;

	TArray<FName> AfterLabels;
	TArray<FName> BeforeLabels;

	bool bWasAdded = false;

	TMap<FName, FArcSystemBuilder> Systems;

};

struct ARCECS_API FArcScheduleBuilder
{
private:

	TMap<FName, FArcScheduleStage> Stages;
	bool bWasScheduleBuilt = false;

	FName CurrentStage = NAME_None;
	FName CurrentSystem = NAME_None;

public:

	FArcScheduleBuilder();
	
	FArcScheduleBuilder& AddStage(const FName& StageLabel);
	FArcScheduleBuilder& AddStageAfter(const FName& StageLabel, const FName& TargetStageLabel);
	FArcScheduleBuilder& AddStageBefore(const FName& StageLabel, const FName& TargetStageLabel);

	FArcScheduleBuilder& AddSystem(const FArcSystemBuilder& System);
	FArcScheduleBuilder& AddSystemSeq(const FArcSystemBuilder& System);
	FArcScheduleBuilder& AddSystemToStage(const FName& StageLabel, const FArcSystemBuilder& System);

	FArcScheduleBuilder& AddSystemSet(const FArcSystemSet& SystemSet);
	FArcScheduleBuilder& AddSystemSetToStage(const FName& StageLabel, const FArcSystemSet& SystemSet);
	
	FArcSchedule BuildSchedule();

private:

	FArcScheduleStage& AddStageInternal(const FName& StageLabel);
	TArray<FArcScheduleStage*> GenerateOrderedStageArray();
	TArray<FArcSystemBuilder*> GenerateOrderedSystemArray(FArcScheduleStage& Stage);
	static void GatherNamesAndLabels(const FArcScheduleStage& Stage, TSet<FName>& OutSystemNames, TMap<FName, int32>& OutSystemLabels);
};
