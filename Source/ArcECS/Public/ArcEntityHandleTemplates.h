// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcEntityHandleInternal.h"
#include "ArcUniverse.h"

template<typename T>
inline bool FArcEntityHandle::HasComponent(const FArcUniverse& Universe) const
{
    return Universe.HasComponent<T>(*this);
}

template<typename T>
inline T* FArcEntityHandle::GetComponent(const FArcUniverse& Universe) const
{
    return Universe.GetComponent<T>(*this);
}
