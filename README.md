# ArcECS

This is a simple ECS plugin for Unreal. The syntax was inspired by [Bevy](https://github.com/bevyengine/bevy) and [Legion](https://github.com/amethyst/legion).

## Code Example

```cpp
struct FTestDeltaTime
{
	float Seconds;
};

struct FTestPosition
{
	FVector Position;
};

struct FTestVelocity
{
	FVector Velocity;
};

struct FTestSystems
{
	static void ApplyVelocity(FTestPosition& Position, FTestVelocity& Velocity, FArcResource<FTestDeltaTime> DeltaTime)
	{
		Position.Position += Velocity.Velocity * DeltaTime->Seconds;
	}

	static void LogPosition(FArcEntityHandle& Entity, FTestPosition& Position)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entity %d, Position %s"), Entity.GetIndex(), *Position.Position.ToString());
	}
};

UCLASS()
class TEST_API ATestGameState : public AGameStateBase
{
	GENERATED_BODY()

private:

	FArcWorld World;
	FArcSchedule TickSchedule;

public:

	// ...

	virtual void PostInitializeComponents() override
	{
		Super::PostInitializeComponents();
	
		World.AddResource(FTestDeltaTime { 0.0f });
	
		TickSchedule = FArcScheduleBuilder()
			.AddSystem(ArcSystem(TEXT("ApplyVelocity"), &FTestSystems::ApplyVelocity))
			.AddSystem(ArcSystem(TEXT("LogPosition"), &FTestSystems::LogPosition)
				.After(TEXT("ApplyVelocity")))
			.BuildSchedule();

		FArcEntityBuilder()
			.AddComponent(FTestPosition { FVector(0.0f, 0.0f, 0.0f) })
			.AddComponent(FTestVelocity { FVector(10.0f, 10.0f, 0.0f) })
			.SpawnEntity(World);
	}

	virtual void Tick(float DeltaSeconds) override
	{
		Super::Tick(DeltaSeconds);

		if (FTestDeltaTime* DeltaTime = World.GetResource<FTestDeltaTime>())
		{
			DeltaTime->Seconds = DeltaSeconds;
		}
		TickSchedule.Execute(World);
	}

   // ...
};
```

## Entities and Components

Components can be attached to entities during or after creation.
```cpp
FArcEntityHandle Entity = FArcEntityBuilder()
	.AddComponent<FTestPosition>(FTestPosition { FVector(0.0f, 0.0f, 0.0f) })
	.SpawnEntity(World);

World.AddComponent(Entity, FTestVelocity { FVector(10.0f, 20.0f, 30.0f) });
World.RemoveComponent<FTestPosition>(Entity);
```

## Resources

Any kind of data can be added as a resource to the `FArcWorld` and then be used within systems.
```cpp
World.AddResource(FTestDeltaTime { 0.0f });

void TestSystem(FArcResource<FTestDeltaTime> DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime %f"), DeltaTime->Seconds);
}
```

## Systems

Systems are functions that are called for all entities that have the required set of components or exactly once if the function signature doesn't contain any entity specific data. Besides components a system can also request any resource, an`FArcEntityHandle` or the `FArcWorld`.
```cpp
// Position is required, Velocity is optional
void TestSystem(FTestPosition& Position, FTestVelocity* Velocity, FArcResource<FTestDeltaTime> DeltaTime)
{
	if (Velocity)
	{
		Position.Position += Velocity->Velocity * DeltaTime->Seconds;
	}
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime %f"), DeltaTime->Seconds);
}
```
## Schedule

Schedules are used to organize systems. When adding systems `Before` and `After` dependencies can be assigned. `AddSystemSeq` can be used as a shorthand to add an `After` dependency to the previously added system.
```cpp
FArcSchedule Schedule = FArcScheduleBuilder()
	.AddSystem(ArcSystem(TEXT("System_1"), &FTestSystems::System_1))
	.AddSystem(ArcSystem(TEXT("System_2"), &FTestSystems::System_2))
		.Before(TEXT("System_1")
		.After(TEXT("System_3")
	.AddSystem(ArcSystem(TEXT("System_3"), &FTestSystems::System_3))
	.AddSystemSeq(ArcSystem(TEXT("System_4"), &FTestSystems::System_4))
	.BuildSchedule();
// Resolved system order should be: System_3, System_4, System_2, System_1
```

## SystemsSets

SystemSets can be used to apply properties to multiple Systems at once.
```cpp
ScheduleBuilder
	.AddSystem(ArcSystem(TEXT("StartPlacement"), &FTestSystems::StartPlacement))
	.AddSystemSet(FArcSystemSet()
		.WithLabel(TEXT("Placement_Tick"))
		.After(TEXT("StartPlacement"))
		.AddSystem(ArcSystem(TEXT("AdjustRotation"), &FTestSystems::AdjustRotation))
		.AddSystemSeq(ArcSystem(TEXT("SnapToGrid"), &FTestSystems::SnapToGrid))
	)
	.AddSystem(ArcSystem(TEXT("TryPlace"), &FTestSystems::TryPlace))
		.After(TEXT("Placement_Tick");
```

## Stages

Each schedule contains a `DefaultStage`, but additional stages can be created if necessary.
```cpp
ScheduleBuilder
	.AddStageBefore(TEXT("PreparationStage"), FArcScheduleStage::DefaultStage)
	.AddStageAfter(TEXT("CleanUpStage"), FArcScheduleStage::DefaultStage)
	.AddSystemToStage(TEXT("CleanUpStage"), ArcSystem(TEXT("ClearEvents"), 
&FTestSystems::ClearEvents))
	.AddSystemSetToStage(TEXT("PreparationStage"), FArcSystemSet(TEXT("PreparationSystems")
		.AddSystem(ArcSystem(TEXT("System_1"), &FTestSystems::System_1))
		.AddSystem(ArcSystem(TEXT("System_2"), &FTestSystems::System_2))
	);
```
