// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcEntityBuilder.h"
#include "ArcUniverse.h"

FArcEntitySignature FArcEntityBuilder::ComputeEntitySignature() const
{
    FArcEntitySignature Signature;
    for (const auto& Entry : Components)
    {
        Signature.Add(Entry.Key);
    }
    return Signature;
}

FArcEntityHandle FArcEntityBuilder::SpawnEntity(FArcUniverse& Universe)
{
    return Universe.SpawnEntity(*this);
}
