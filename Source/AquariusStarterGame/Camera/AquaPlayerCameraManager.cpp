// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/AquaPlayerCameraManager.h"

AAquaPlayerCameraManager::AAquaPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = AQUA_CAMERA_DEFAULT_FOV;
	ViewPitchMin = AQUA_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = AQUA_CAMERA_DEFAULT_PITCH_MAX;
}

void AAquaPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}
