# ArcECS

This is a simple ECS plugin for Unreal. The API was inspired by [Bevy](https://github.com/bevyengine/bevy) and [Legion](https://github.com/amethyst/legion).

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
	static void ApplyVelocity(FTestPosition& Position, FTestVelocity& Velocity, FArcRes<FTestDeltaTime> DeltaTime)
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

	FArcUniverse Universe;
	FArcSchedule TickSchedule;

public:

	// ...

	virtual void PostInitializeComponents() override
	{
		Super::PostInitializeComponents();
	
		Universe.AddResource(FTestDeltaTime { 0.0f });
	
		TickSchedule = FArcScheduleBuilder()
			.AddSystem(ArcSystem(&FTestSystems::ApplyVelocity), FArcSystemConfig(TEXT("ApplyVelocity"))
			.AddSystem(ArcSystem(&FTestSystems::LogPosition, FArcSystemConfig())
				.After(TEXT("ApplyVelocity")))
			.BuildSchedule();

		FArcEntityBuilder()
			.AddComponent(FTestPosition { FVector(0.0f, 0.0f, 0.0f) })
			.AddComponent(FTestVelocity { FVector(10.0f, 10.0f, 0.0f) })
			.SpawnEntity(Universe);
	}

	virtual void Tick(float DeltaSeconds) override
	{
		Super::Tick(DeltaSeconds);

		if (FTestDeltaTime* DeltaTime = Universe.GetResource<FTestDeltaTime>())
		{
			DeltaTime->Seconds = DeltaSeconds;
		}
		TickSchedule.Execute(Universe);
	}

   // ...
};
```

## Entities and Components

Components can be attached to entities during or after creation.
```cpp
FArcEntityHandle Entity = FArcEntityBuilder()
	.AddComponent<FTestPosition>(FTestPosition { FVector(0.0f, 0.0f, 0.0f) })
	.SpawnEntity(Universe);

Universe.AddComponent(Entity, FTestVelocity { FVector(10.0f, 20.0f, 30.0f) });
Universe.RemoveComponent<FTestPosition>(Entity);
```

## Resources

Any kind of data can be added as a resource to the `FArcUniverse` and then be used within systems.
```cpp
Universe.AddResource(FTestDeltaTime { 0.0f });

void TestSystem(FArcRes<FTestDeltaTime> DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime %f"), DeltaTime->Seconds);
}
```

## Systems

Systems are functions that are called for all entities that have the required set of components or exactly once if the function signature doesn't contain any entity specific data. Besides components a system can also request any resource, an`FArcEntityHandle` or the `FArcUniverse`.
```cpp
// Position is required, Velocity is optional
void TestSystem(FTestPosition& Position, FTestVelocity* Velocity, FArcRes<FTestDeltaTime> DeltaTime)
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
	.AddSystem(&FTestSystems::System_1, FArcSystemConfig(TEXT("System_1")))
	.AddSystem(&FTestSystems::System_2, FArcSystemConfig()
		.Before(TEXT("System_1")
		.After(TEXT("System_3"))
	.AddSystem(&FTestSystems::System_3, FArcSystemConfig(TEXT("System_3")))
	.AddSystemSeq(&FTestSystems::System_4))
	.BuildSchedule();
// Resolved system order should be: System_3, System_4, System_2, System_1
```

## SystemsSets

SystemSets can be used to apply properties to multiple Systems at once.
```cpp
ScheduleBuilder
	.AddSystem(&FTestSystems::StartPlacement, FArcSystemConfig(TEXT("StartPlacement")))
	.AddSystemSet(FArcSystemSet()
		.WithLabel(TEXT("Placement_Tick"))
		.After(TEXT("StartPlacement"))
		.AddSystem(&FTestSystems::AdjustRotation, FArcSystemConfig(TEXT("AdjustRotation")))
		.AddSystemSeq(&FTestSystems::SnapToGrid, FArcSystemConfig(TEXT("SnapToGrid")))
	)
	.AddSystem(&FTestSystems::TryPlace, FArcSystemConfig(TEXT("TryPlace"))
		.After(TEXT("Placement_Tick")));
```

## Stages

Each schedule contains a `DefaultStage`, but additional stages can be created if necessary.
```cpp
ScheduleBuilder
	.AddStageBefore(TEXT("PreparationStage"), FArcScheduleStage::DefaultStage)
	.AddStageAfter(TEXT("CleanUpStage"), FArcScheduleStage::DefaultStage)
	.AddSystemToStage(TEXT("CleanUpStage"), &FTestSystems::ClearEvents)
	.AddSystemSetToStage(TEXT("PreparationStage"), FArcSystemSet()
		.WithLabel(TEXT("PreparationSystems"))
		.AddSystem(&FTestSystems::System_1)
		.AddSystem(&FTestSystems::System_2)
	);
```
