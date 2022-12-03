#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ArcECSSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Arc ECS Settings"))
class ARCECS_API UArcECSSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<class UArcECSSetup> ECSSetupClass;

	static const UArcECSSettings& Get() { return *GetDefault<UArcECSSettings>(); }
};
