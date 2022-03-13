// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcEntityHandleInternal.h"
#include "ArcWorld.h"

bool FArcEntityHandle::IsValid(const FArcWorld& World) const
{
    return World.IsValid(*this);
}
