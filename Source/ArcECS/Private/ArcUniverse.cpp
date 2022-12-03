// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcUniverse.h"
#include "ArcEntityBuilder.h"

void FArcUniverse::Lock()
{
    bLocked = true;
}

void FArcUniverse::Unlock()
{
    bLocked = false;
}

FArcCommands& FArcUniverse::GetCommands()
{
    return Commands;
}

void FArcUniverse::ExecuteCommands()
{
    for (TSharedPtr<FArcCommand>& Command : Commands.Commands)
    {
        if (!Command) { continue; }
        Command->Execute(*this);
    }
    Commands.Commands.Reset();
}

FArcEntityHandle FArcUniverse::SpawnEntity(FArcEntityBuilder& EntityBuilder)
{
    if (!ensureMsgf(!bLocked, TEXT("Attempting to spawn an entity during iteration!")))
    {
        return FArcEntityHandle();
    }
    
    const uint16 Index = [&]()
    {
        if (FreeIndices.Num() > 0)
        {
            return FreeIndices.Pop();
        }
        EntityDatas.Emplace();
        return static_cast<uint16>(EntityDatas.Num() -1);
    }();

    const FArcEntityHandle Entity = FArcEntityHandle(Index, EntityDatas[Index].Generation);

    const FArcEntitySignature Signature = EntityBuilder.ComputeEntitySignature();
    TSharedPtr<FArcArchetypeContainer>& Container = ArchetypeContainers.FindOrAdd(Signature, MakeShared<FArcArchetypeContainer>());
    for (const auto& entry : EntityBuilder.Components)
    {
        entry.Value->AddToArchetypeContainer(*Container);
    }
    Container->EntityHandles.Add(Entity);

    EntityDatas[Index].Container = &*Container;
    EntityDatas[Index].IndexInContainer = Container->EntityHandles.Num() - 1;
    
    return Entity;
}

void FArcUniverse::DeleteEntity(const FArcEntityHandle& Entity)
{
    if (!ensureMsgf(!bLocked, TEXT("Attempting to delete an entity during iteration!")))
    {
        return;
    }
    
    if (!IsValid(Entity)) { return; }
    FArcEntityData& EntityData = EntityDatas[Entity.GetIndex()];
    FArcArchetypeContainer& Container = *EntityData.Container;
    if (!ensure(Container.EntityHandles.Num() > 0)) { return; }

    const FArcEntityHandle& EntityToRedirect = Container.EntityHandles.Last();
    EntityDatas[EntityToRedirect.GetIndex()].IndexInContainer = EntityData.IndexInContainer;

    Container.SwapRemove(EntityData.IndexInContainer);
    if (Container.IsEmpty())
    {
        ArchetypeContainers.Remove(Container.ComputeEntitySignature());
    }
    EntityData.Container = nullptr;
    ++EntityData.Generation;

    FreeIndices.Add(Entity.GetIndex());
}

bool FArcUniverse::IsValid(const FArcEntityHandle& Entity) const
{
    return
        EntityDatas.IsValidIndex(Entity.GetIndex()) &&
        Entity.GetGeneration() == EntityDatas[Entity.GetIndex()].Generation &&
        ensure(EntityDatas[Entity.GetIndex()].Container != nullptr);
}
