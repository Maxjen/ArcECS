// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcArchetypeContainer.h"
#include "ArcEntityHandleInternal.h"
#include "ArcTypeID.h"
#include "ArcEntitySignature.h"

class FArcComponentWrapperBase
{
public:
    virtual ~FArcComponentWrapperBase() {}
    virtual void AddToArchetypeContainer(FArcArchetypeContainer& Container) {}
};

template<typename T>
class FArcComponentWrapper : public FArcComponentWrapperBase
{
public:

    T Component;
    FArcComponentWrapper(T&& InComponent) : Component(MoveTemp(InComponent)) {}
    virtual void AddToArchetypeContainer(FArcArchetypeContainer& Container) override
    {
        FArcComponentArray<T>& ComponentArray = Container.FindOrAddComponentArray<T>();
        ComponentArray.Components.Add(MoveTemp(Component));
    }
};

class ARCECS_API FArcEntityBuilder
{
public:

    TMap<FArcTypeID, TSharedPtr<FArcComponentWrapperBase>> Components;

public:

    template<typename T>
    FArcEntityBuilder& AddComponent(T&& Component)
    {
        Components.Emplace(FArcTypeIDHelper::Get<T>(), MakeShared<FArcComponentWrapper<T>>(MoveTemp(Component)));
        return *this;
    }

    FArcEntitySignature ComputeEntitySignature() const;
    FArcEntityHandle SpawnEntity(class FArcUniverse& Universe);

};
