// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcSchedule.h"
#include "ArcWorld.h"

FArcSchedule::FArcSchedule(TArray<TSharedPtr<FArcSystemInternalBase>>& InSystems)
{
    Systems = InSystems;
}

void FArcSchedule::Execute(FArcWorld& World) const
{
    for (const auto& System : Systems)
    {
        System->Execute(World);
    }
}
