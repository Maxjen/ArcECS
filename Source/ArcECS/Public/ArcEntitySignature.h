// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcTypeID.h"

using FArcEntitySignature = TSet<FArcTypeID>;

inline uint32 GetTypeHash(const FArcEntitySignature& Signature)
{
    uint32 Result = 0;
    for (const FArcTypeID& TypeID : Signature)
    {
        Result = HashCombine(Result, GetTypeHash(TypeID));
    }
    return Result;
}

inline bool operator==(const FArcEntitySignature& SignatureA, const FArcEntitySignature& SignatureB)
{
    if (SignatureA.Num() != SignatureB.Num()) { return false; }
    for (const FArcTypeID& TypeID : SignatureA)
    {
        if (!SignatureB.Contains(TypeID)) { return false; }
    }
    return true;
}
