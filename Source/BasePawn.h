// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePawn.generated.h"

UCLASS()
class TGW_TANKS_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();
	
	void HandleDestruction();

protected:
	// Called when the game starts or when spawned

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float GridSize = 100.f;

	FVector DeltaLocation;

	void Fire();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void SetProjectilesInOneBurst(int32 Value);
	void SetProjectilesDelayTime(float Value);
	void SetSpeed(float Value);
	float GetSpeed();

	UFUNCTION(BlueprintCallable)
	void SetProjectileClass(TSubclassOf<class AProjectile> NewProjectileClass);

private:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//class UPaperSpriteComponent* PaperSprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 ProjectilesInOneBurst = 1;
	int32 ProjectilesFired = 0;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ProjectilesDelayTime = 0.f;


	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;

	void SpawnProjectiles();
	float GetMyProjectilesInSceneCount();

	FTimerHandle TimerHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};