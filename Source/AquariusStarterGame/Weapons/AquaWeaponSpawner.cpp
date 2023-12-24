#include "Weapons/AquaWeaponSpawner.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AquaLogChannels.h"
#include "NavigationSystemTypes.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/AquaPickupDefinition.h"
#include "Inventory/AquaInventoryItemDefinition.h"
#include "..\Inventory\InventoryFragment_SetStats.h"

// Sets default values
AAquaWeaponSpawner::AAquaWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	CollisionVolume->InitCapsuleSize(80.f, 80.f);
	
	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	PadMesh->SetupAttachment(RootComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponMeshRotationSpeed = 40.f;
	CoolDownTime = 30.f;
	CheckExistingOverlapDelay = 0.25f;
	bIsWeaponAvailable = true;
}

// Called when the game starts or when spawned
void AAquaWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AAquaWeaponSpawner::OnOverlapBegin);
    
	if (WeaponPickupDefinition && WeaponPickupDefinition->InventoryItemDefinition)
	{
		CoolDownTime = WeaponPickupDefinition->SpawnCoolDownSeconds;
	}
	else
	{
		UE_LOG(LogAqua, Error, TEXT("'%s' does not have a valid weapon definition! Make sure to set this data on the instance!"), *GetNameSafe(this));
	}
}

void AAquaWeaponSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CoolDownTimerHandle);
		World->GetTimerManager().ClearTimer(CheckOverlapDelayTimerHandle);
	}
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AAquaWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update the CoolDownPercentage property to drive respawn time indicators
	UWorld* World = GetWorld();
	if (World->GetTimerManager().IsTimerActive(CoolDownTimerHandle))
	{
		CoolDownPercentage = 1.f - World->GetTimerManager().GetTimerRemaining(CoolDownTimerHandle)/CoolDownTime;
	}

	WeaponMesh->AddRelativeRotation(FRotator(0.f, World->GetDeltaSeconds() * WeaponMeshRotationSpeed, 0.f));
}

void AAquaWeaponSpawner::OnConstruction(const FTransform& Transform)
{
	if (WeaponPickupDefinition != nullptr && WeaponPickupDefinition->DisplayMesh != nullptr)
	{
		WeaponMesh->SetStaticMesh(WeaponPickupDefinition->DisplayMesh);
		WeaponMesh->SetRelativeLocation(WeaponPickupDefinition->WeaponMeshOffset);
		WeaponMesh->SetRelativeScale3D(WeaponPickupDefinition->WeaponMeshScale);
	}
}

void AAquaWeaponSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("on overlap begin"));
	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (GetLocalRole() == ROLE_Authority && bIsWeaponAvailable && OverlappingPawn != nullptr)
	{
		AttemptPickupWeapon(OverlappingPawn);
	}
}

void AAquaWeaponSpawner::CheckForExistingOverlaps()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		AttemptPickupWeapon(Cast<APawn>(OverlappingActor));
	}
}

void AAquaWeaponSpawner::StartCoolDown()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CoolDownTimerHandle, this, &AAquaWeaponSpawner::OnCoolDownTimerComplete, CoolDownTime);
	}
}

void AAquaWeaponSpawner::ResetCoolDown()
{
	UWorld* World = GetWorld();

	if (World)
	{
		World->GetTimerManager().ClearTimer(CoolDownTimerHandle);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		bIsWeaponAvailable = true;
		SetWeaponPickupVisibility(true);

		if (World)
		{
			World->GetTimerManager().SetTimer(CheckOverlapDelayTimerHandle, this, &AAquaWeaponSpawner::CheckForExistingOverlaps, CheckExistingOverlapDelay);
		}
	}
}

void AAquaWeaponSpawner::OnCoolDownTimerComplete()
{
	ResetCoolDown();
}

void AAquaWeaponSpawner::SetWeaponPickupVisibility(bool bShouldBeVisible)
{
	WeaponMesh->SetVisibility(bShouldBeVisible, true);
}

int32 AAquaWeaponSpawner::GetDefaultStatFromItemDef(const TSubclassOf<UAquaInventoryItemDefinition> WeaponItemClass,
	FGameplayTag StatTag)
{
	if (WeaponItemClass != nullptr)
	{
		if (UAquaInventoryItemDefinition* WeaponItemCDO = WeaponItemClass->GetDefaultObject<UAquaInventoryItemDefinition>())
		{
			if (const UInventoryFragment_SetStats* ItemStatsFragment = Cast<UInventoryFragment_SetStats>(WeaponItemCDO->FindFragmentByClass(UInventoryFragment_SetStats::StaticClass())))
			{
				return ItemStatsFragment->GetItemStatByTag(StatTag);
			}
		}
	}

	return 0;
}

void AAquaWeaponSpawner::AttemptPickupWeapon_Implementation(APawn* Pawn)
{
	if (GetLocalRole() == ROLE_Authority && bIsWeaponAvailable && UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
	{
		TSubclassOf<UAquaInventoryItemDefinition> WeaponItemDefinition = WeaponPickupDefinition ? WeaponPickupDefinition->InventoryItemDefinition : nullptr;
		if (WeaponItemDefinition != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("weapon name: %s"), *WeaponItemDefinition->GetName());
			//Attempt to grant the weapon
			if (GiveWeapon(WeaponItemDefinition, Pawn))
			{
				//Weapon picked up by pawn
				bIsWeaponAvailable = false;
				SetWeaponPickupVisibility(false);
				StartCoolDown();
			}
		}
	}
}
