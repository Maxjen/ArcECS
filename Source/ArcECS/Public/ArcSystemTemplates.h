// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcWorld.h"
#include "ArcRes.h"

template<typename T>
struct FArcRequiredTypeIDHelper
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcWorld& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        bOutCallForEachEntity = true;
        return FArcTypeIDHelper::Get<T>();
    }
};

template<typename T>
struct FArcRequiredTypeIDHelper<T*>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcWorld& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        bOutCallForEachEntity = true;
        return TOptional<FArcTypeID>();
    }
};

template<>
struct FArcRequiredTypeIDHelper<FArcEntityHandle&>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcWorld& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        bOutCallForEachEntity = true;
        return TOptional<FArcTypeID>();
    }
};

template<typename T>
struct FArcRequiredTypeIDHelper<FArcRes<T>>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcWorld& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        if (!World.GetResource<T>())
        {
            bOutResourcesAvailable = false;
        }
        return TOptional<FArcTypeID>();
    }
};

template<>
struct FArcRequiredTypeIDHelper<FArcWorld>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcWorld& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        return TOptional<FArcTypeID>();
    }
};

template<typename T>
struct FArcGetArgumentForIDHelper
{
    static T& GetArgument(FArcWorld& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return *Container->GetComponent<std::remove_reference_t<T>>(Index);
    }
};

template<typename T>
struct FArcGetArgumentForIDHelper<T*>
{
    static T* GetArgument(FArcWorld& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return Container->GetComponent<T>(Index);
    }
};

template<>
struct FArcGetArgumentForIDHelper<FArcEntityHandle&>
{
    static FArcEntityHandle& GetArgument(FArcWorld& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return *Container->GetEntityHandle(Index);
    }
};

template<typename T>
struct FArcGetArgumentForIDHelper<FArcRes<T>>
{
    static FArcRes<T> GetArgument(FArcWorld& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return FArcRes<T>(World.GetResource<T>());
    }
};

template<>
struct FArcGetArgumentForIDHelper<FArcWorld&>
{
    static FArcWorld& GetArgument(FArcWorld& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return World;
    }
};
