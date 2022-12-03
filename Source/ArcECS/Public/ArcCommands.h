// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcCommandsInternal.h"
#include "ArcUniverse.h"

template<typename T>
class FArcAddComponentCommand : public FArcCommand
{
public:

	FArcEntityHandle Entity;
	T Component;
	
public:
	FArcAddComponentCommand(const FArcEntityHandle& InEntity, T&& InComponent) : Entity(InEntity), Component(MoveTemp(InComponent)) {}
	
	virtual void Execute(FArcUniverse& Universe) override
	{
		Universe.AddComponent(Entity, MoveTemp(Component));
	}
};

template<typename T>
class FArcRemoveComponentCommand : public FArcCommand
{
public:

	FArcEntityHandle Entity;
	
public:
	FArcRemoveComponentCommand(const FArcEntityHandle& InEntity) : Entity(InEntity) {}
	
	virtual void Execute(FArcUniverse& Universe) override
	{
		Universe.RemoveComponent<T>(Entity);
	}
};

template<typename T>
void FArcCommands::AddComponent(const FArcEntityHandle& Entity, T&& Component)
{
	Commands.Add(MakeShared<FArcAddComponentCommand<T>>(Entity, MoveTemp(Component)));
}

template<typename T>
void FArcCommands::RemoveComponent(const FArcEntityHandle& Entity)
{
	Commands.Add(MakeShared<FArcRemoveComponentCommand<T>>(Entity));
}
