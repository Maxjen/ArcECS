// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcEntityHandleInternal.h"
#include "ArcUniverse.h"

bool FArcEntityHandle::IsValid(const FArcUniverse& Universe) const
{
    return Universe.IsValid(*this);
}
