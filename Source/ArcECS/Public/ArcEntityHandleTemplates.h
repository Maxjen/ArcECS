// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcEntityHandleInternal.h"
#include "ArcWorld.h"

template<typename T>
inline bool FArcEntityHandle::HasComponent(const FArcWorld& World) const
{
    return World.HasComponent<T>(*this);
}

template<typename T>
inline T* FArcEntityHandle::GetComponent(const FArcWorld& World) const
{
    return World.GetComponent<T>(*this);
}
