// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <typeindex>

using FArcTypeID = size_t;

struct FArcTypeIDHelper
{
    template<typename T>
    static FArcTypeID Get()
    {
        std::type_index TypeIndex = std::type_index(typeid(T));
        return *reinterpret_cast<size_t*>(&TypeIndex);
    }
};
