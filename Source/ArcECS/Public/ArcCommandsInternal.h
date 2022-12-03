// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcEntityBuilder.h"

class FArcCommand
{
public:
    virtual ~FArcCommand() {}
	virtual void Execute(class FArcUniverse& Universe) {}
};

class ARCECS_API FArcCommands
{
public:
	
    TArray<TSharedPtr<FArcCommand>> Commands;
    
public:

    void SpawnEntity(class FArcEntityBuilder& EntityBuilder);
    void DeleteEntity(const FArcEntityHandle& Entity);

	template<typename T>
	void AddComponent(const FArcEntityHandle& Entity, T&& Component);
	
	template<typename T>
	void RemoveComponent(const FArcEntityHandle& Entity);
};
