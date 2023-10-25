// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/AquaWeaponSpawner.h"

// Sets default values
AAquaWeaponSpawner::AAquaWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAquaWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAquaWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

