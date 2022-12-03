// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcEntityHandleInternal.h"
#include "ArcTypeID.h"
#include "ArcEntitySignature.h"

class FArcComponentArrayBase
{
public:
    virtual ~FArcComponentArrayBase() {}
    virtual void SwapRemove(uint16 Index) {}
    virtual void MoveToContainer(uint16 Index, class FArcArchetypeContainer& Container) {}
};

template<typename T>
class FArcComponentArray : public FArcComponentArrayBase
{
public:

    TArray<T> Components;

    virtual void SwapRemove(uint16 Index) override
    {
        Components.RemoveAtSwap(Index);
    }

    virtual void MoveToContainer(uint16 Index, class FArcArchetypeContainer& Container) override;
};

class ARCECS_API FArcArchetypeContainer
{
public:

    TArray<FArcEntityHandle> EntityHandles;
    TMap<FArcTypeID, TSharedPtr<FArcComponentArrayBase>> ComponentArrays;

public:
    
    FArcEntitySignature ComputeEntitySignature() const;
    bool IsEmpty() const;
    void SwapRemove(uint16 Index);

    FArcEntityHandle* GetEntityHandle(uint16 Index);

    template<typename T>
    FArcComponentArray<T>& FindOrAddComponentArray()
    {
        TSharedPtr<FArcComponentArrayBase>& ComponentArray = ComponentArrays.FindOrAdd(
            FArcTypeIDHelper::Get<T>(),
            MakeShared<FArcComponentArray<T>>()
        );
        return *static_cast<FArcComponentArray<T>*>(ComponentArray.Get());
    }

    template<typename T>
    bool HasComponent() const
    {
        return ComponentArrays.Contains(FArcTypeIDHelper::Get<T>());
    }

    template<typename T>
    T* GetComponent(uint16 Index) const
    {
        if (!EntityHandles.IsValidIndex(Index)) { return nullptr; }
        const TSharedPtr<FArcComponentArrayBase>* ArrayPtr = ComponentArrays.Find(FArcTypeIDHelper::Get<T>());
        if (!ArrayPtr) { return nullptr; }
        FArcComponentArray<T>& ComponentArray = *static_cast<FArcComponentArray<T>*>(ArrayPtr->Get());
        return &ComponentArray.Components[Index];
    }
};

template <typename T>
void FArcComponentArray<T>::MoveToContainer(uint16 Index, FArcArchetypeContainer& Container)
{
    FArcComponentArray<T>& ComponentArray = Container.FindOrAddComponentArray<T>();
    ComponentArray.Components.Add(MoveTemp(Components[Index]));
}
