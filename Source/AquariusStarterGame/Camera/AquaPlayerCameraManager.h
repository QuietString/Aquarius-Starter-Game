#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "AquaPlayerCameraManager.generated.h"


#define AQUA_CAMERA_DEFAULT_FOV			(80.0f)
#define AQUA_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define AQUA_CAMERA_DEFAULT_PITCH_MAX	(89.0f)


/**
 * AAqualayerCameraManager
 *
 *	The base player camera manager class used by this project.
 */
UCLASS(NotPlaceable, MinimalAPI)
class AAquaPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AAquaPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
