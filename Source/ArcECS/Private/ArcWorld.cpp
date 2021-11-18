// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcWorld.h"
#include "ArcEntityBuilder.h"

FArcEntityHandle FArcWorld::SpawnEntity(FArcEntityBuilder& EntityBuilder)
{
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

void FArcWorld::DeleteEntity(const FArcEntityHandle& Entity)
{
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

bool FArcWorld::IsValid(const FArcEntityHandle& Entity) const
{
    return
        EntityDatas.IsValidIndex(Entity.GetIndex()) &&
        Entity.GetGeneration() == EntityDatas[Entity.GetIndex()].Generation &&
        ensure(EntityDatas[Entity.GetIndex()].Container != nullptr);
}
