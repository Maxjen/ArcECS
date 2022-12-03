// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <typeindex>

using FArcTypeID = size_t;

struct FArcTypeIDHelper
{
    template<typename T>
    static FArcTypeID Get()
    {
        return typeid(T).hash_code();
    }
};
