// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcUniverse.h"
#include "ArcRes.h"

template<typename T>
struct FArcRequiredTypeIDHelper
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        bOutCallForEachEntity = true;
        return FArcTypeIDHelper::Get<T>();
    }
};

template<typename T>
struct FArcRequiredTypeIDHelper<T*>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        bOutCallForEachEntity = true;
        return TOptional<FArcTypeID>();
    }
};

template<>
struct FArcRequiredTypeIDHelper<FArcEntityHandle&>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        bOutCallForEachEntity = true;
        return TOptional<FArcTypeID>();
    }
};

template<typename T>
struct FArcRequiredTypeIDHelper<FArcRes<T>>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        if (!World.GetResource<T>())
        {
            bOutResourcesAvailable = false;
        }
        return TOptional<FArcTypeID>();
    }
};

template<>
struct FArcRequiredTypeIDHelper<FArcUniverse&>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        return TOptional<FArcTypeID>();
    }
};

template<>
struct FArcRequiredTypeIDHelper<FArcCommands&>
{
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        return TOptional<FArcTypeID>();
    }
};

template<typename T>
struct FArcGetArgumentForIDHelper
{
    static T& GetArgument(FArcUniverse& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return *Container->GetComponent<std::remove_reference_t<T>>(Index);
    }
};

template<typename T>
struct FArcGetArgumentForIDHelper<T*>
{
    static T* GetArgument(FArcUniverse& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return Container->GetComponent<T>(Index);
    }
};

template<>
struct FArcGetArgumentForIDHelper<FArcEntityHandle&>
{
    static FArcEntityHandle& GetArgument(FArcUniverse& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return *Container->GetEntityHandle(Index);
    }
};

template<typename T>
struct FArcGetArgumentForIDHelper<FArcRes<T>>
{
    static FArcRes<T> GetArgument(FArcUniverse& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return FArcRes<T>(World.GetResource<T>());
    }
};

template<>
struct FArcGetArgumentForIDHelper<FArcUniverse&>
{
    static FArcUniverse& GetArgument(FArcUniverse& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return World;
    }
};

template<>
struct FArcGetArgumentForIDHelper<FArcCommands&>
{
    static FArcCommands& GetArgument(FArcUniverse& World, FArcArchetypeContainer* Container, int32 Index)
    {
        return World.GetCommands();
    }
};
