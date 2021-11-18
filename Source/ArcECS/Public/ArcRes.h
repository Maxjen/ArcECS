// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

template<typename T>
class FArcRes
{
private:

    T* Resource;

public:

    FArcRes(T* InResource) : Resource(InResource) {}

    T* operator->() const
    {
        return Resource;
    }

    T& operator*() const
    {
        return *Resource;
    }

};
