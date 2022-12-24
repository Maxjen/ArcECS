#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ArcECSSetup.generated.h"

UCLASS(Abstract)
class ARCECS_API UArcECSSetup : public UObject
{
	GENERATED_BODY()

public:

	virtual void SetupECSForGame(class FArcUniverse& Universe, class UWorld& World, struct FArcScheduleBuilder& InitScheduleBuilder, struct FArcScheduleBuilder& TickScheduleBuilder) {}
	virtual void SetupECSForServer(class FArcUniverse& Universe, class UWorld& World, struct FArcScheduleBuilder& InitScheduleBuilder, struct FArcScheduleBuilder& TickScheduleBuilder) {}
	virtual void SetupECSForEditor(class FArcUniverse& Universe, class UWorld& World, struct FArcScheduleBuilder& InitScheduleBuilder, struct FArcScheduleBuilder& TickScheduleBuilder) {}
};
