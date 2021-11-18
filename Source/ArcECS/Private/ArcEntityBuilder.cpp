// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcEntityBuilder.h"
#include "ArcWorld.h"

FArcEntitySignature FArcEntityBuilder::ComputeEntitySignature() const
{
    FArcEntitySignature Signature;
    for (const auto& Entry : Components)
    {
        Signature.Add(Entry.Key);
    }
    return Signature;
}

FArcEntityHandle FArcEntityBuilder::SpawnEntity(FArcWorld& World)
{
    return World.SpawnEntity(*this);
}
