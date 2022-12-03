#include "ActorComponent/ArcBaseComponent.h"
#include "ArcECSSubsystem.h"
#include "Resource/ArcCoreData.h"

void UArcBaseComponent::OnRegister()
{
    Super::OnRegister();

    UArcECSSubsystem* ECSSubsystem = UWorld::GetSubsystem<UArcECSSubsystem>(GetWorld());
    AActor* Owner = GetOwner();
    if (ECSSubsystem && Owner)
    {
        FArcEntityHandle& Entity = ECSSubsystem->FindOrCreateEntityForActor(*Owner);
        FArcUniverse& Universe = ECSSubsystem->GetUniverse();
        AddComponentsToEntity(Universe, Entity);
    }
}

void UArcBaseComponent::OnUnregister()
{
    Super::OnUnregister();
    
    UArcECSSubsystem* ECSSubsystem = UWorld::GetSubsystem<UArcECSSubsystem>(GetWorld());
    AActor* Owner = GetOwner();
    if (ECSSubsystem && Owner)
    {
        FArcEntityHandle* Entity = ECSSubsystem->FindEntityForActor(*Owner);
        if (Entity)
        {
            FArcUniverse& Universe = ECSSubsystem->GetUniverse();
            RemoveComponentsFromEntity(Universe, *Entity);
        }
    }
}

FArcEntityHandle UArcBaseComponent::GetEntityHandle() const
{
    UArcECSSubsystem* ECSSubsystem = UWorld::GetSubsystem<UArcECSSubsystem>(GetWorld());
    FArcCoreData* CoreData = ECSSubsystem ? ECSSubsystem->GetUniverse().GetResource<FArcCoreData>() : nullptr;
    const FArcEntityHandle* Result = CoreData ? CoreData->EntitiesByActor.Find(GetOwner()) : nullptr;
    return Result ? *Result : FArcEntityHandle();
}
