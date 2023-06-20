// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "EnemyTank.generated.h"

/**
 * 
 */
UCLASS()
class TGW_TANKS_API AEnemyTank : public ABasePawn
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();
	int32 GetScoreReward() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class ATank* Tank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float FireRange = 300.f;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 ScoreReward = 100;

	FTimerHandle FireRateTimerHandle;
	float FireRate = 2.f;
	void CheckFireCondition();
	float TimeElapsed;
	void MoveRandomly(float DeltaTime);
	int MovementValue = FMath::RandRange(0, 2) - 1;
	float TimeToSwitchMovement = FMath::FRandRange(2, 6);
	bool bMoveForward = FMath::RandBool();

	bool InFireRange();

};


