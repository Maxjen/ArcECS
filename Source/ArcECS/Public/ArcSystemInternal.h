// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcSystemTemplates.h"
#include "ArcUniverse.h"

class FArcSystemInternalBase
{
public:
    virtual ~FArcSystemInternalBase() {}
    virtual void Execute(FArcUniverse& World) {}
};

template<typename... Args>
class FArcSystemInternal : public FArcSystemInternalBase
{
public:

    TFunction<void(Args...)> Function;

public:

    virtual void Execute(FArcUniverse& World) override
    {
        bool bResourcesAvailable = true;
        bool bCallForEachEntity = false;
        const FArcEntitySignature MinimalSignature = GetMinimalSignature(World, bCallForEachEntity, bResourcesAvailable);
        if (!bResourcesAvailable) { return; }

        if (bCallForEachEntity)
        {
            World.Lock();
            for (const auto& Entry : World.ArchetypeContainers)
            {
                const FArcEntitySignature& ContainerSignature = Entry.Key;
                bool bContainsMinimalSignature = true;
                for (const FArcTypeID& TypeID : MinimalSignature)
                {
                    if (!ContainerSignature.Contains(TypeID))
                    {
                        bContainsMinimalSignature = false;
                        break;
                    }
                }
                if (!bContainsMinimalSignature) { continue; }

                FArcArchetypeContainer& Container = *Entry.Value;
                for (int32 i = 0; i < Container.EntityHandles.Num(); ++i)
                {
                    Function(FArcGetArgumentForIDHelper<Args>::GetArgument(World, &Container, i)...);
                }
            }
            World.Unlock();
        }
        else
        {
            Function(FArcGetArgumentForIDHelper<Args>::GetArgument(World, nullptr, 0)...);
        }
    }

    static FArcEntitySignature GetMinimalSignature(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        FArcEntitySignature Signature;
        AppendTypeIDsRecursive(Signature, GetRequiredTypeID<Args>(World, bOutCallForEachEntity, bOutResourcesAvailable)...);
        return Signature;
    }

    template<typename... Types>
    static void AppendTypeIDsRecursive(FArcEntitySignature& Signature) {}
    
    template<typename... Types>
    static void AppendTypeIDsRecursive(FArcEntitySignature& Signature, TOptional<FArcTypeID> FirstArg, Types... OtherArgs)
    {
        if (FirstArg)
        {
            Signature.Add(*FirstArg);
        }
        AppendTypeIDsRecursive(Signature, OtherArgs...);
    }

    template<typename T>
    static TOptional<FArcTypeID> GetRequiredTypeID(FArcUniverse& World, bool& bOutCallForEachEntity, bool& bOutResourcesAvailable)
    {
        return FArcRequiredTypeIDHelper<T>::GetRequiredTypeID(World, bOutCallForEachEntity, bOutResourcesAvailable);
    }
};
