// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "PaperSpriteComponent.h"

// Sets default values
ABasePawn::ABasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	//PaperSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Paper Sprite"));
	//PaperSprite->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);
}

void ABasePawn::HandleDestruction()
{
	//TODO: Visual/sound 
	

};

void ABasePawn::SetProjectilesInOneBurst(int32 Value)
{
	ProjectilesInOneBurst = Value;
}

void ABasePawn::SetProjectilesDelayTime(float Value)
{
	ProjectilesDelayTime = Value;
}

void ABasePawn::SetSpeed(float Value)
{
	Speed = Value;
}

float ABasePawn::GetSpeed()
{
	return Speed;
}

void ABasePawn::SpawnProjectiles()
{
	// Check if we've reached the maximum value
	if (ProjectilesFired >= ProjectilesInOneBurst)
	{
		// Stop the timer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		ProjectilesFired = 0;
		return;
	}
	ProjectilesFired++;

	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);
}

void ABasePawn::SetProjectileClass(TSubclassOf<class AProjectile> NewProjectileClass)
{
	ProjectileClass = NewProjectileClass;
}

float ABasePawn::GetMyProjectilesInSceneCount()
{
	TArray<AActor*> AllProjectiles;
	UGameplayStatics::GetAllActorsOfClass(this, AProjectile::StaticClass(), AllProjectiles);

	int32 MyProjectilesCount = 0;

	for (auto AllProjectilesItr = AllProjectiles.CreateIterator(); AllProjectilesItr; AllProjectilesItr++)
	{
		AActor* ProjectileActor = *AllProjectilesItr;
		auto ProjectileOwner = ProjectileActor->GetOwner();
		bool IsMyChild = ProjectileOwner && (ProjectileOwner == this);
		if (IsMyChild)
		{
			MyProjectilesCount++;
		}
	}

	return MyProjectilesCount;
}

void ABasePawn::Fire()
{
	if (GetMyProjectilesInSceneCount() <= 0)
	{
		SpawnProjectiles();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABasePawn::SpawnProjectiles, ProjectilesDelayTime, true);
	}
}

// Called every frame
void ABasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePawn::MoveForward(float Value)
{
	DeltaLocation.X += Value * Speed * GridSize * UGameplayStatics::GetWorldDeltaSeconds(this);

	FVector CurrentGridLocation = GetActorLocation();

	FVector NewLocation = CurrentGridLocation + DeltaLocation;
	NewLocation.X = FMath::RoundToInt(NewLocation.X / GridSize) * GridSize;
	NewLocation.Y = FMath::RoundToInt(NewLocation.Y / GridSize) * GridSize;

	if (NewLocation.X != CurrentGridLocation.X) {
		FHitResult HitResult;
		SetActorLocation(NewLocation, true, &HitResult);
		DeltaLocation = FVector::ZeroVector;

		if (HitResult.IsValidBlockingHit()) {
			SetActorLocation(CurrentGridLocation);
		}
	}

	if (NewLocation.X > CurrentGridLocation.X) {
		SetActorRotation(FRotator(0, 0, 0));
	}
	else if (NewLocation.X < CurrentGridLocation.X) {
		SetActorRotation(FRotator(0, 180, 0));
	}
}

void ABasePawn::MoveRight(float Value)
{
	DeltaLocation.Y += Value * Speed * GridSize * UGameplayStatics::GetWorldDeltaSeconds(this);

	FVector CurrentGridLocation = GetActorLocation();
	FVector NewLocation = CurrentGridLocation + DeltaLocation;

	NewLocation.Y = FMath::RoundToInt(NewLocation.Y / GridSize) * GridSize;

	if (NewLocation.Y != CurrentGridLocation.Y) {
		FHitResult HitResult;
		SetActorLocation(NewLocation, true, &HitResult);
		DeltaLocation = FVector::ZeroVector;

		if (HitResult.IsValidBlockingHit()) {
			SetActorLocation(CurrentGridLocation);
		}

		SetActorRotation(FRotator(0, 90 * Value, 0));

	}
}