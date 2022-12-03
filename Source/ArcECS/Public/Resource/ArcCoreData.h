#pragma once

#include "ArcEntityHandle.h"

struct ARCECS_API FArcCoreData
{
	TWeakObjectPtr<UWorld> World;
	TMap<TWeakObjectPtr<AActor>, FArcEntityHandle> EntitiesByActor;
	TMap<FArcEntityHandle, TWeakObjectPtr<AActor>> ActorsByEntity;
};
