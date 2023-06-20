// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTank.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void AEnemyTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveRandomly(DeltaTime);
}

void AEnemyTank::MoveRandomly(float DeltaTime)
{
	TimeElapsed += DeltaTime;

	if (bMoveForward)
	{
		MoveForward(MovementValue);
	}
	else
	{
		MoveRight(MovementValue);
	}

	if (TimeElapsed >= TimeToSwitchMovement)
	{
		TimeElapsed = 0;
		TimeToSwitchMovement = FMath::FRandRange(2, 6);
		MovementValue = FMath::RandRange(0, 2) - 1;
		bMoveForward = FMath::RandBool();
	}
}

void AEnemyTank::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

int32 AEnemyTank::GetScoreReward() const
{
	return ScoreReward;
}

void AEnemyTank::BeginPlay()
{
	Super::BeginPlay();

	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AEnemyTank::CheckFireCondition, FireRate, true);
}

void AEnemyTank::CheckFireCondition()
{
	if (InFireRange())
	{
		Fire();
	}
}

bool AEnemyTank::InFireRange()
{
	if (Tank)
	{
		float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());

		if (Distance <= FireRange)
		{
			return true;
		}
	}

	return false;
}