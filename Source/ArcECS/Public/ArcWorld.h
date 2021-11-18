// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcArchetypeContainer.h"
#include "ArcEntityHandle.h"
#include "ArcTypeID.h"

class ARCECS_API FArcWorld
{
public:

	TMap<FArcEntitySignature, TSharedPtr<FArcArchetypeContainer>> ArchetypeContainers;
	
private:
	
	struct FArcEntityData
	{
		uint16 Generation = 0;
		uint16 IndexInContainer = 0;
		FArcArchetypeContainer* Container = nullptr;
	};
	
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

	TArray<FArcEntityData> EntityDatas;
	TArray<uint16> FreeIndices;

	TMap<FArcTypeID, TSharedPtr<FArcResourceWrapperBase>> Resources;

public:

	FArcEntityHandle SpawnEntity(class FArcEntityBuilder& EntityBuilder);
	void DeleteEntity(const FArcEntityHandle& Entity);

	bool IsValid(const FArcEntityHandle& Entity) const;

	template<typename T>
	FArcWorld& AddResource(T&& Resource)
	{
		Resources.Emplace(FArcTypeIDHelper::Get<T>(), MakeShared<FArcResourceWrapper<T>>(MoveTemp(Resource)));
		return *this;
	}

	template<typename T>
	T* GetResource()
	{
		if (TSharedPtr<FArcResourceWrapperBase>* Wrapper = Resources.Find(FArcTypeIDHelper::Get<T>()))
		{
			FArcResourceWrapper<T>& WrapperCasted = *static_cast<FArcResourceWrapper<T>*>(Wrapper->Get());
			return &WrapperCasted.Resource;
		}
		return nullptr;
	}

	template<typename T>
	bool HasComponent(const FArcEntityHandle& Entity) const
	{
		if (!IsValid(Entity)) { return false; }
		const FArcArchetypeContainer& Container = *EntityDatas[Entity.GetIndex()].Container;
		return Container.HasComponent<T>();
	}

	template<typename T>
	T* GetComponent(const FArcEntityHandle& Entity) const
	{
		if (!IsValid(Entity)) { return nullptr; }
		const FArcEntityData& EntityData = EntityDatas[Entity.GetIndex()];
		const FArcArchetypeContainer& Container = *EntityData.Container;
		return Container.GetComponent<T>(EntityData.IndexInContainer);
	}

	template<typename T>
	void AddComponent(const FArcEntityHandle& Entity, T&& Component)
	{
		if (!IsValid(Entity) || HasComponent<T>(Entity)) { return; }

		FArcEntityData& EntityData = EntityDatas[Entity.GetIndex()];
		FArcArchetypeContainer& Container = *EntityData.Container;
		if (!ensure(!Container.IsEmpty())) { return; }

		const FArcTypeID& ToAdd = FArcTypeIDHelper::Get<T>();
		const FArcEntitySignature OldSignature = Container.ComputeEntitySignature();
		FArcEntitySignature NewSignature = OldSignature;
		NewSignature.Add(ToAdd);

		const FArcEntityHandle& EntityToRedirect = Container.EntityHandles.Last();
		EntityDatas[EntityToRedirect.GetIndex()].IndexInContainer = EntityData.IndexInContainer;

		TSharedPtr<FArcArchetypeContainer>& NewContainer = ArchetypeContainers.FindOrAdd(NewSignature, MakeShared<FArcArchetypeContainer>());
		for (auto& Entry : Container.ComponentArrays)
		{
			TSharedPtr<FArcComponentArrayBase>& ComponentArray = Entry.Value;
			ComponentArray->MoveToContainer(EntityData.IndexInContainer, *NewContainer);
		}

		Container.SwapRemove(EntityData.IndexInContainer);
		if (Container.IsEmpty())
		{
			ArchetypeContainers.Remove(OldSignature);
		}

		FArcComponentArray<T>& ComponentArray = NewContainer->FindOrAddComponentArray<T>();
		ComponentArray.Components.Add(Component);
		NewContainer->EntityHandles.Add(Entity);

		EntityData.Container = NewContainer.Get();
		EntityData.IndexInContainer = NewContainer->EntityHandles.Num() - 1;
	}

	template<typename T>
	void RemoveComponent(const FArcEntityHandle& Entity)
	{
		if (!IsValid(Entity) || !HasComponent<T>(Entity)) { return; }
		
		FArcEntityData& EntityData = EntityDatas[Entity.GetIndex()];
		FArcArchetypeContainer& Container = *EntityData.Container;
		if (!ensure(!Container.IsEmpty())) { return; }
		
		const FArcTypeID& ToRemove = FArcTypeIDHelper::Get<T>();
		const FArcEntitySignature OldSignature = Container.ComputeEntitySignature();
		FArcEntitySignature NewSignature = OldSignature;
		NewSignature.Remove(ToRemove);
		
		const FArcEntityHandle& EntityToRedirect = Container.EntityHandles.Last();
		EntityDatas[EntityToRedirect.GetIndex()].IndexInContainer = EntityData.IndexInContainer;
		
		TSharedPtr<FArcArchetypeContainer>& NewContainer = ArchetypeContainers.FindOrAdd(NewSignature, MakeShared<FArcArchetypeContainer>());
		for (auto& Entry : Container.ComponentArrays)
		{
			TSharedPtr<FArcComponentArrayBase>& ComponentArray = Entry.Value;
			if (Entry.Key == ToRemove) { continue; }
			ComponentArray->MoveToContainer(EntityData.IndexInContainer, *NewContainer);
		}
		
		Container.SwapRemove(EntityData.IndexInContainer);
		if (Container.IsEmpty())
		{
			ArchetypeContainers.Remove(OldSignature);
		}
		
		NewContainer->EntityHandles.Add(Entity);

		EntityData.Container = NewContainer.Get();
		EntityData.IndexInContainer = NewContainer->EntityHandles.Num() - 1;
	}
};
