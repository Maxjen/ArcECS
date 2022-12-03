// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcSystem.h"

FArcSystemConfig::FArcSystemConfig(const FName& Label)
{
    Labels.Add(Label);
}

FArcSystemConfig& FArcSystemConfig::WithLabel(const FName& Label)
{
    Labels.AddUnique(Label);
    return *this;
}

FArcSystemConfig& FArcSystemConfig::After(const FName& Label)
{
    AfterLabels.AddUnique(Label);
    return *this;
}

FArcSystemConfig& FArcSystemConfig::Before(const FName& Label)
{
    BeforeLabels.AddUnique(Label);
    return *this;
}

FArcSystemBuilder& FArcSystemBuilder::WithLabel(const FName& Label)
{
    Labels.AddUnique(Label);
    return *this;
}

FArcSystemBuilder& FArcSystemBuilder::After(const FName& Label)
{
    AfterLabels.AddUnique(Label);
    return *this;
}

FArcSystemBuilder& FArcSystemBuilder::Before(const FName& Label)
{
    BeforeLabels.AddUnique(Label);
    return *this;
}

FName FArcSystemBuilder::GetName() const
{
    if (Labels.Num() < 1)
    {
        UE_LOG(LogTemp, Fatal, TEXT("System has no name!"));
    }
    return Labels[0];
}

FArcSystemSet& FArcSystemSet::WithLabel(const FName& Label)
{
    Labels.AddUnique(Label);
    return *this;
}

FArcSystemSet& FArcSystemSet::After(const FName& Label)
{
    AfterLabels.AddUnique(Label);
    return *this;
}

FArcSystemSet& FArcSystemSet::Before(const FName& Label)
{
    BeforeLabels.AddUnique(Label);
    return *this;
}

FArcSystemSet& FArcSystemSet::AddSystem(const FArcSystemBuilder& System)
{
    Systems.Add(System);
    CurrentSystem = System.GetName();
    return *this;
}

FArcSystemSet& FArcSystemSet::AddSystemSeq(const FArcSystemBuilder& System)
{
    if (ensureMsgf(CurrentSystem != NAME_None, TEXT("AddSystemSeq should only be called after AddSystem, AddSystemSeq or AddSystemToStage!")))
    {
        FArcSystemBuilder SystemToAdd = System;
        SystemToAdd.AfterLabels.AddUnique(CurrentSystem);
        AddSystem(System);
    }
    else
    {
        AddSystem(System);
    }
    return *this;
}
