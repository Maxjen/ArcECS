#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcBaseComponent.generated.h"

UCLASS(Abstract, DefaultToInstanced, EditInlineNew, HideCategories = (Activation, AssetUserData, Collision, ComponentTick, ComponentReplication, Cooking, Replication, Tags, Variable))
class ARCECS_API UArcBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	virtual void AddComponentsToEntity(class FArcUniverse& Universe, class FArcEntityHandle& Entity) {}
	virtual void RemoveComponentsFromEntity(class FArcUniverse& Universe, class FArcEntityHandle& Entity) {}

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	FArcEntityHandle GetEntityHandle() const;

};
