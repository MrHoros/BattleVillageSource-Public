// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class TGW_TANKS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float Damage = 50.f;
	float BlastRadiusY = 20.f;
	float BlastRadiusX = 10.f;
	class ATGW_TanksGameModeBase* TanksGameMode;

	void DamageTile(FVector Direction, int32 TileX, int32 TileY);

	class APaperTileMapActor* CurrentLevelTileMap;
	class UPaperTileMapComponent* TileMapComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
