// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcArchetypeContainer.h"

FArcEntitySignature FArcArchetypeContainer::ComputeEntitySignature() const
{
    FArcEntitySignature Signature;
    for (const auto& Entry : ComponentArrays)
    {
        Signature.Add(Entry.Key);
    }
    return Signature;
}

bool FArcArchetypeContainer::IsEmpty() const
{
    return EntityHandles.Num() <= 0;
}

void FArcArchetypeContainer::SwapRemove(uint16 Index)
{
    if (!EntityHandles.IsValidIndex(Index)) { return; }
    EntityHandles.RemoveAtSwap(Index);
    for (auto& Entry : ComponentArrays)
    {
        Entry.Value->SwapRemove(Index);
    }
}

FArcEntityHandle* FArcArchetypeContainer::GetEntityHandle(uint16 Index)
{
    if (!EntityHandles.IsValidIndex(Index)) { return nullptr; }
    return &EntityHandles[Index];
}
