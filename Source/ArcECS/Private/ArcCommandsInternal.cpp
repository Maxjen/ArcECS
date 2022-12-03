// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcCommandsInternal.h"
#include "ArcUniverse.h"

class FArcSpawnEntityCommand : public FArcCommand
{
public:
	FArcEntityBuilder EntityBuilder;

public:

	FArcSpawnEntityCommand(const FArcEntityBuilder& InEntityBuilder) : EntityBuilder(InEntityBuilder) {}
	
	virtual void Execute(class FArcUniverse& World) override
	{
		World.SpawnEntity(EntityBuilder);
	}
};

class FArcDeleteEntityCommand : public FArcCommand
{
public:
	FArcEntityHandle Entity;
	
public:

	FArcDeleteEntityCommand(const FArcEntityHandle& InEntity) : Entity(InEntity) {}
	
	virtual void Execute(class FArcUniverse& World) override
	{
		World.DeleteEntity(Entity);
	}
};

void FArcCommands::SpawnEntity(class FArcEntityBuilder& EntityBuilder)
{
	Commands.Add(MakeShared<FArcSpawnEntityCommand>(EntityBuilder));
}

void FArcCommands::DeleteEntity(const FArcEntityHandle& Entity)
{
	Commands.Add(MakeShared<FArcDeleteEntityCommand>(Entity));
}
