#include "ArcECSSubsystem.h"
#include "ArcECSSettings.h"
#include "ArcECSSetup.h"
#include "ArcScheduleBuilder.h"
#include "Actor/ArcScheduleExecutor.h"
#include "Resource/ArcCoreData.h"

void UArcECSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();
	if (!World) { return; }

	Universe.AddResource<FArcCoreData>({ World });

	if (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview)
	{
		if (const UClass* SetupClass = UArcECSSettings::Get().ECSSetupClass.LoadSynchronous())
		{
			UArcECSSetup* Setup = NewObject<UArcECSSetup>(this, SetupClass);
			FArcScheduleBuilder TickScheduleBuilder;
			FArcScheduleBuilder InitScheduleBuilder;
			Setup->SetupECSForEditor(Universe, *World, InitScheduleBuilder, TickScheduleBuilder);
			const FArcSchedule EditorInitSchedule = InitScheduleBuilder.BuildSchedule();
			EditorInitSchedule.Execute(Universe);
			EditorTickSchedule = TickScheduleBuilder.BuildSchedule();
		}
	}
	
	if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
	{
		if (const UClass* SetupClass = UArcECSSettings::Get().ECSSetupClass.LoadSynchronous())
		{
			UArcECSSetup* Setup = NewObject<UArcECSSetup>(this, SetupClass);
			FArcScheduleBuilder InitScheduleBuilder;
			FArcScheduleBuilder TickScheduleBuilder;
			if (World->IsNetMode(ENetMode::NM_Client) || World->IsNetMode(ENetMode::NM_Standalone))
			{
				Setup->SetupECSForGame(Universe, *World, InitScheduleBuilder, TickScheduleBuilder);
			}
			else if (World->IsNetMode(ENetMode::NM_DedicatedServer))
			{
				Setup->SetupECSForServer(Universe, *World, InitScheduleBuilder, TickScheduleBuilder);
			}
			GameInitSchedule = InitScheduleBuilder.BuildSchedule();
			AArcScheduleExecutor::SpawnScheduleExecutor(*World, TickScheduleBuilder);
		}
	}
}

void UArcECSSubsystem::OnWorldBeginPlay(UWorld& World)
{
	Super::OnWorldBeginPlay(World);

	if (World.WorldType == EWorldType::Game || World.WorldType == EWorldType::PIE)
	{
		GameInitSchedule.Execute(Universe);
	}
}

void UArcECSSubsystem::Tick(float DeltaTime)
{
	const UWorld* World = GetWorld();
	if (!World) { return; }
	
	if (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview)
	{
		EditorTickSchedule.Execute(Universe);
	}
}

FArcEntityHandle& UArcECSSubsystem::FindOrCreateEntityForActor(AActor& Actor)
{
	FArcCoreData* CoreData = Universe.GetResource<FArcCoreData>();
	check(CoreData);

	if (FArcEntityHandle* Entity = CoreData->EntitiesByActor.Find(&Actor))
	{
		return *Entity;
	}
	FArcEntityBuilder EntityBuilder;
	Actor.OnDestroyed.AddUniqueDynamic(this, &UArcECSSubsystem::OnActorDestroyed);
	const FArcEntityHandle Entity = Universe.SpawnEntity(EntityBuilder);
	CoreData->ActorsByEntity.Add(Entity, &Actor);
	return CoreData->EntitiesByActor.FindOrAdd(&Actor, Entity);
}

FArcEntityHandle* UArcECSSubsystem::FindEntityForActor(AActor& Actor)
{
	FArcCoreData* CoreData = Universe.GetResource<FArcCoreData>();
	return CoreData ? CoreData->EntitiesByActor.Find(&Actor) : nullptr;
}

UArcECSSubsystem* UArcECSSubsystem::Get(const UObject* Context)
{
	return UWorld::GetSubsystem<UArcECSSubsystem>(Context ? Context->GetWorld() : nullptr);
}

bool UArcECSSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::Editor || WorldType == EWorldType::PIE || WorldType == EWorldType::EditorPreview;
}

void UArcECSSubsystem::OnActorDestroyed(AActor* Actor)
{
	FArcCoreData* CoreData = Universe.GetResource<FArcCoreData>();
	FArcEntityHandle* Entity = CoreData ? CoreData->EntitiesByActor.Find(Actor) : nullptr;
	if (Entity)
	{
		Universe.DeleteEntity(*Entity);
		CoreData->EntitiesByActor.Remove(Actor);
	}
}
