// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcSchedule.h"
#include "ArcSystem.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArcECS, Log, All);

struct FArcScheduleStageConfig
{
public:

	FName StageLabel;

	TArray<FName> AfterLabels;
	TArray<FName> BeforeLabels;

	bool bWasAdded = false;

	TMap<FName, FArcSystemBuilder> Systems;

};

struct ARCECS_API FArcScheduleBuilder
{
private:

	TMap<FName, FArcScheduleStageConfig> Stages;
	bool bWasScheduleBuilt = false;

	FName CurrentStage = NAME_None;
	FName CurrentSystem = NAME_None;

public:

	FArcScheduleBuilder();
	
	FArcScheduleBuilder& AddUnrealStages();
	FArcScheduleBuilder& AddStage(const FName& StageLabel);
	FArcScheduleBuilder& AddStageAfter(const FName& StageLabel, const FName& TargetStageLabel);
	FArcScheduleBuilder& AddStageBefore(const FName& StageLabel, const FName& TargetStageLabel);

	FArcScheduleBuilder& AddSystem(const FArcSystemBuilder& System);
	FArcScheduleBuilder& AddSystemSeq(const FArcSystemBuilder& System);
	FArcScheduleBuilder& AddSystemToStage(const FName& StageLabel, const FArcSystemBuilder& System);

	FArcScheduleBuilder& AddSystemSet(const FArcSystemSet& SystemSet);
	FArcScheduleBuilder& AddSystemSetToStage(const FName& StageLabel, const FArcSystemSet& SystemSet);
	
	template<typename... Args>
	FArcScheduleBuilder& AddSystem(void(*Function)(Args...))
	{
		return AddSystem(ArcSystem(Function));
	}
	
	template<typename... Args>
	FArcScheduleBuilder& AddSystem(void(*Function)(Args...), const FArcSystemConfig& Config)
	{
		FArcSystemBuilder SystemBuilder = ArcSystem(Function);
		if (Config.Labels.Num() > 0)
		{
			SystemBuilder.Labels = Config.Labels;
		}
		SystemBuilder.AfterLabels = Config.AfterLabels;
		SystemBuilder.BeforeLabels = Config.BeforeLabels;
		return AddSystem(SystemBuilder);
	}
	
	template<typename... Args>
	FArcScheduleBuilder& AddSystemSeq(void(*Function)(Args...))
	{
		return AddSystemSeq(ArcSystem(Function));
	}
	
	template<typename... Args>
	FArcScheduleBuilder& AddSystemSeq(void(*Function)(Args...), const FArcSystemConfig& Config)
	{
		FArcSystemBuilder SystemBuilder = ArcSystem(Function);
		if (Config.Labels.Num() > 0)
		{
			SystemBuilder.Labels = Config.Labels;
		}
		SystemBuilder.AfterLabels = Config.AfterLabels;
		SystemBuilder.BeforeLabels = Config.BeforeLabels;
		return AddSystemSeq(SystemBuilder);
	}
	
	template<typename... Args>
	FArcScheduleBuilder& AddSystemToStage(const FName& StageLabel, void(*Function)(Args...))
	{
		return AddSystemToStage(StageLabel, ArcSystem(Function));
	}
	
	template<typename... Args>
	FArcScheduleBuilder& AddSystemToStage(const FName& StageLabel, void(*Function)(Args...), const FArcSystemConfig& Config)
	{
		FArcSystemBuilder SystemBuilder = ArcSystem(Function);
		if (Config.Labels.Num() > 0)
		{
			SystemBuilder.Labels = Config.Labels;
		}
		SystemBuilder.AfterLabels = Config.AfterLabels;
		SystemBuilder.BeforeLabels = Config.BeforeLabels;
		return AddSystemToStage(StageLabel, SystemBuilder);
	}
	
	FArcSchedule BuildSchedule();

private:

	FArcScheduleStageConfig& AddStageInternal(const FName& StageLabel);
	TArray<FArcScheduleStageConfig*> GenerateOrderedStageArray();
	static TArray<FArcSystemBuilder*> GenerateOrderedSystemArray(FArcScheduleStageConfig& Stage);
	static void GatherNamesAndLabels(const FArcScheduleStageConfig& Stage, TSet<FName>& OutSystemNames, TMap<FName, int32>& OutSystemLabels);
};
