#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcStageExecutorComponent.generated.h"

UCLASS()
class UArcStageExecutorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	
	int32 FirstStageIndex = INDEX_NONE;
	int32 LastStageIndex = INDEX_NONE;

public:

	UArcStageExecutorComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void SetStageRange(int32 InFirstStageIndex, int32 InLastStageIndex);

};
