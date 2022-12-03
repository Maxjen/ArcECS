#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ArcSchedule.h"
#include "ArcUniverse.h"
#include "ArcECSSubsystem.generated.h"

UCLASS()
class ARCECS_API UArcECSSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

private:

	FArcUniverse Universe;
	
	FArcSchedule GameInitSchedule;
	
	FArcSchedule EditorTickSchedule;
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& World) override;

	// FTickableGameObject
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UArcECSSubsystem, STATGROUP_Tickables); }
	virtual void Tick(float DeltaTime) override;
	// ~FTickableGameObject
	
	FArcEntityHandle& FindOrCreateEntityForActor(AActor& Actor);
	FArcEntityHandle* FindEntityForActor(AActor& Actor);

	FArcUniverse& GetUniverse() { return Universe; }

	static UArcECSSubsystem* Get(const UObject* Context);

protected:
	
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	
	UFUNCTION()
	void OnActorDestroyed(AActor* Actor);

};
