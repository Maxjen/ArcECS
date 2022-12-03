// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcScheduleBuilder.h"

DEFINE_LOG_CATEGORY(LogArcECS);

FArcScheduleBuilder::FArcScheduleBuilder()
{
    AddStage(FArcScheduleStage::DefaultStage);
}

FArcScheduleBuilder& FArcScheduleBuilder::AddUnrealStages()
{
    AddStageAfter(FArcScheduleStage::PrePhysicsStage, FArcScheduleStage::DefaultStage);
    AddStageAfter(FArcScheduleStage::DuringPhysicsStage, FArcScheduleStage::PrePhysicsStage);
    AddStageAfter(FArcScheduleStage::PostPhysicsStage, FArcScheduleStage::DuringPhysicsStage);
    AddStageAfter(FArcScheduleStage::PostUpdateWorkStage, FArcScheduleStage::PostPhysicsStage);
    AddStageAfter(FArcScheduleStage::LastDemotableStage, FArcScheduleStage::PostUpdateWorkStage);
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddStage(const FName& StageLabel)
{
    AddStageInternal(StageLabel);
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddStageAfter(const FName& StageLabel, const FName& TargetStageLabel)
{
    FArcScheduleStageConfig& Stage = AddStageInternal(StageLabel);
    Stage.AfterLabels.Add(TargetStageLabel);
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddStageBefore(const FName& StageLabel, const FName& TargetStageLabel)
{
    FArcScheduleStageConfig& Stage = AddStageInternal(StageLabel);
    Stage.BeforeLabels.Add(TargetStageLabel);
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddSystem(const FArcSystemBuilder& System)
{
    AddSystemToStage(FArcScheduleStage::DefaultStage, System);
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddSystemSeq(const FArcSystemBuilder& System)
{
    if (ensureMsgf(CurrentStage != NAME_None && CurrentSystem != NAME_None, TEXT("AddSystemSeq should only be called after AddSystem, AddSystemSeq or AddSystemToStage!")))
    {
        FArcSystemBuilder SystemToAdd = System;
        SystemToAdd.AfterLabels.AddUnique(CurrentSystem);
        AddSystemToStage(CurrentStage, SystemToAdd);
    }
    else
    {
        AddSystem(System);
    }
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddSystemToStage(const FName& StageLabel, const FArcSystemBuilder& System)
{
    FArcScheduleStageConfig& Stage = Stages.FindOrAdd(StageLabel);
    if (Stage.Systems.Contains(System.GetName()))
    {
        UE_LOG(LogArcECS, Fatal, TEXT("Trying to add system with already existing name '%s'!"), *System.GetName().ToString());
    }
    Stage.Systems.Add(System.GetName(), System);
    
    CurrentStage = StageLabel;
    CurrentSystem = System.GetName();
    
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddSystemSet(const FArcSystemSet& SystemSet)
{
    AddSystemSetToStage(FArcScheduleStage::DefaultStage, SystemSet);
    return *this;
}

FArcScheduleBuilder& FArcScheduleBuilder::AddSystemSetToStage(const FName& StageLabel, const FArcSystemSet& SystemSet)
{
    CurrentStage = NAME_None;
    CurrentSystem = NAME_None;
    
    for (const FArcSystemBuilder& System : SystemSet.Systems)
    {
        FArcSystemBuilder SystemToAdd = System;
        for (const FName& Label : SystemSet.Labels)
        {
            SystemToAdd.Labels.AddUnique(Label);
        }
        for (const FName& Label : SystemSet.AfterLabels)
        {
            SystemToAdd.AfterLabels.AddUnique(Label);
        }
        for (const FName& Label : SystemSet.BeforeLabels)
        {
            SystemToAdd.BeforeLabels.AddUnique(Label);
        }
        AddSystemToStage(StageLabel, SystemToAdd);
    }
    return *this;
}

FArcSchedule FArcScheduleBuilder::BuildSchedule()
{
    if (bWasScheduleBuilt)
    {
        UE_LOG(LogArcECS, Fatal, TEXT("BuildSchedule must not be called more than once on the same object!"))
    }
    bWasScheduleBuilt = true;
    
    TArray<FArcScheduleStageConfig*> StagesOrdered = GenerateOrderedStageArray();

    FArcSchedule Schedule;

    for (int32 i = 0; i < StagesOrdered.Num(); ++i)
    {
        FArcScheduleStageConfig* Stage = StagesOrdered[i];
        FString SystemsString = TEXT("[ ");
        TArray<FArcSystemBuilder*> SystemsOrdered = GenerateOrderedSystemArray(*Stage);
        TArray<TSharedPtr<FArcSystemInternalBase>> StageSystems;
        for (const FArcSystemBuilder* System : SystemsOrdered)
        {
            SystemsString.Append(FString::Printf(TEXT("%s, "), *System->GetName().ToString()));
            StageSystems.Add(System->SystemInternal);
        }
        SystemsString.Append(TEXT(" ]"));
        
        UE_LOG(LogArcECS, Warning, TEXT("Stage %d: '%s'"), i, *Stage->StageLabel.ToString());
        UE_LOG(LogArcECS, Warning, TEXT("%s"), *SystemsString);

        Schedule.AddStage(Stage->StageLabel, StageSystems);
    }
    
    return Schedule;
}

FArcScheduleStageConfig& FArcScheduleBuilder::AddStageInternal(const FName& StageLabel)
{
    CurrentStage = NAME_None;
    CurrentSystem = NAME_None;
    
    FArcScheduleStageConfig& Stage = Stages.FindOrAdd(StageLabel);
    if (Stage.bWasAdded)
    {
        UE_LOG(LogArcECS, Fatal, TEXT("Trying to add already existing stage '%s'!"), *StageLabel.ToString());
    }
    Stage.StageLabel = StageLabel;
    Stage.bWasAdded = true;
    return Stage;
}

TArray<FArcScheduleStageConfig*> FArcScheduleBuilder::GenerateOrderedStageArray()
{
    for (const auto& Entry : Stages)
    {
        if (!Entry.Value.bWasAdded)
        {
            UE_LOG(LogArcECS, Fatal, TEXT("Stage '%s' does not exist!"), *Entry.Key.ToString());
        }
    }
    
    for (const auto& Entry : Stages)
    {
        for (const FName& After : Entry.Value.AfterLabels)
        {
            if (!Stages.Contains(After))
            {
                UE_LOG(LogArcECS, Fatal, TEXT("Trying to add stage '%s' after non-existing stage '%s'!"), *Entry.Key.ToString(), *After.ToString());
            }
        }
        for (const FName& Before : Entry.Value.BeforeLabels)
        {
            if (!Stages.Contains(Before))
            {
                UE_LOG(LogArcECS, Fatal, TEXT("Trying to add stage '%s' before non-existing stage '%s'!"), *Entry.Key.ToString(), *Before.ToString());
            }
            Stages[Before].AfterLabels.AddUnique(Entry.Key);
        }
    }

    TSet<FName> AddedStages;
    TArray<FArcScheduleStageConfig*> StagesOrdered;
    bool bChanged = true;
    while (bChanged)
    {
        bChanged = false;
        for (auto& Entry : Stages)
        {
            if (AddedStages.Contains(Entry.Key)) { continue; }
            const bool bAllRequiredStagesAdded = [&]()
            {
                for (const FName& After : Entry.Value.AfterLabels)
                {
                    if (!AddedStages.Contains(After)) { return false; }
                }
                return true;
            }();
            if (bAllRequiredStagesAdded)
            {
                bChanged = true;
                AddedStages.Add(Entry.Key);
                StagesOrdered.Add(&Entry.Value);
            }
        }
    }
    if (StagesOrdered.Num() != Stages.Num())
    {
        UE_LOG(LogArcECS, Fatal, TEXT("Failed to resolve stage order!"));
    }
    
    return StagesOrdered;
}

TArray<FArcSystemBuilder*> FArcScheduleBuilder::GenerateOrderedSystemArray(FArcScheduleStageConfig& Stage)
{
    TSet<FName> AllSystemNames;
    TMap<FName, int32> AllSystemLabels;
    GatherNamesAndLabels(Stage, AllSystemNames, AllSystemLabels);
    
    for (const auto& Entry : Stage.Systems)
    {
        const FArcSystemBuilder& System = Entry.Value;
        for (const FName& After : System.AfterLabels)
        {
            if (!AllSystemLabels.Contains(After))
            {
                UE_LOG(LogArcECS, Fatal, TEXT("Trying to add system '%s' after non-existing label '%s'!"), *Entry.Key.ToString(), *After.ToString());
            }
        }
        for (const FName& Before : System.BeforeLabels)
        {
            if (!AllSystemLabels.Contains(Before))
            {
                UE_LOG(LogArcECS, Fatal, TEXT("Trying to add system '%s' before non-existing label '%s'!"), *Entry.Key.ToString(), *Before.ToString());
            }
            for (auto& OtherEntry : Stage.Systems)
            {
                FArcSystemBuilder& OtherSystem = OtherEntry.Value;
                if (OtherSystem.Labels.Contains(Before))
                {
                    OtherSystem.AfterLabels.AddUnique(System.GetName());
                }
            }
        }
    }
    
    TSet<FName> AddedSystems;
    TArray<FArcSystemBuilder*> SystemsOrdered;
    bool bChanged = true;
    while (bChanged)
    {
        bChanged = false;
        for (auto& Entry : Stage.Systems)
        {
            const FName& SystemName = Entry.Key;
            FArcSystemBuilder& System = Entry.Value;
            if (AddedSystems.Contains(SystemName)) { continue; }
            const bool bAllRequiredSystemsAdded = [&]()
            {
                for (const FName& After : System.AfterLabels)
                {
                    if (AllSystemLabels[After] > 0) { return false; }
                }
                return true;
            }();
            if (bAllRequiredSystemsAdded)
            {
                bChanged = true;
                AddedSystems.Add(SystemName);
                SystemsOrdered.Add(&System);
                for (const FName& Label : System.Labels)
                {
                    --AllSystemLabels[Label];
                }
            }
        }
    }
    if (SystemsOrdered.Num() != Stage.Systems.Num())
    {
        UE_LOG(LogArcECS, Fatal, TEXT("Failed to resolve system order!"));
    }
    
    return SystemsOrdered;
}

void FArcScheduleBuilder::GatherNamesAndLabels(const FArcScheduleStageConfig& Stage, TSet<FName>& OutSystemNames, TMap<FName, int32>& OutSystemLabels)
{
    OutSystemNames.Reset();
    OutSystemLabels.Reset();
    
    for (const auto& Entry : Stage.Systems)
    {
        const FArcSystemBuilder& System = Entry.Value;
        if (OutSystemNames.Contains(Entry.Key))
        {
            UE_LOG(LogArcECS, Fatal, TEXT("Found duplicate system name '%s'!"), *Entry.Key.ToString());
        }
        OutSystemNames.Add(Entry.Key);
        for (const FName& Label : System.Labels)
        {
            ++OutSystemLabels.FindOrAdd(Label, 0);
        }
    }
}
