// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FArcResourceWrapperBase
{
public:
    virtual ~FArcResourceWrapperBase() {}
};

template<typename T>
class FArcResourceWrapper : public FArcResourceWrapperBase
{
public:
    T Resource;
    FArcResourceWrapper(T&& InResource) : Resource(InResource) {}
};

template<typename T>
class FArcResource
{
private:

    T* Resource;

public:

    FArcResource(T* InResource) : Resource(InResource) {}

    T* operator->() const
    {
        return Resource;
    }

    T& operator*() const
    {
        return *Resource;
    }

};
