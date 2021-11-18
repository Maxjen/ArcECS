// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcSystemInternal.h"

class ARCECS_API FArcSchedule
{
private:

    TArray<TSharedPtr<FArcSystemInternalBase>> Systems;

public:

    FArcSchedule() = default;
    FArcSchedule(TArray<TSharedPtr<FArcSystemInternalBase>>& InSystems);

    void Execute(class FArcWorld& World) const;
};
