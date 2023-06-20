// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "TGW_TanksGameModeBase.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	TanksGameMode = Cast<ATGW_TanksGameModeBase>(UGameplayStatics::GetGameMode(this));
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	CurrentLevelTileMap = TanksGameMode->GetCurrentLevelTileMap();
	if (IsValid(CurrentLevelTileMap))
	{
		TileMapComponent = CurrentLevelTileMap->GetRenderComponent();
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::DamageTile(FVector Direction, int32 TileX, int32 TileY)
{
FPaperTileInfo TileInfo = TileMapComponent->GetTile(TileX, TileY, 0);

FPaperTileInfo NewTileInfo = FPaperTileInfo();
int32 TileIndex = TileInfo.GetTileIndex();

NewTileInfo.TileSet = TileInfo.TileSet;

//Eagle hit
if (TileIndex == 1361 || TileIndex == 1362 || TileIndex == 1425 || TileIndex == 1426)
{
	TanksGameMode->KillPlayer();
	return;
}


//Destroyable hit
if (TileIndex <= 63 || (TileIndex >= 640 && TileIndex <= 703) || (TileIndex >= 1280 && TileIndex <= 1359)) //Rows with full tiles
{
	//Change tile to damaged tile based on direction
	/* e.x:
		x>=0 then damage the tile from bottom -> 64x6 (row with only top sides)
		x<0 then damage the tile from top -> 64x8 (row with only bottom sides)

		y>=0 then damage the tile from left -> 64x4 (row with only right sides)
		y<0 then damage the tile from right-> 64x2 (row with only left sides)
	*/
	if (Direction.X > 0)
	{
		NewTileInfo.PackedTileIndex = TileIndex + 256;
		UE_LOG(LogTemp, Warning, TEXT("BOT damaged. Old tile index: %i. New index: %i."), TileIndex, NewTileInfo.GetTileIndex());

	}
	else if (Direction.X < 0)
	{
		NewTileInfo.PackedTileIndex = TileIndex + 128;
		UE_LOG(LogTemp, Warning, TEXT("TOP damaged. Old tile index: %i. New index: %i."), TileIndex, NewTileInfo.GetTileIndex());
	}
	else if (Direction.Y > 0)
	{
		NewTileInfo.PackedTileIndex = TileIndex + 512;
		UE_LOG(LogTemp, Warning, TEXT("LEFT damaged. Old tile index: %i. New index: %i."), TileIndex, NewTileInfo.GetTileIndex());
	}
	else
	{
		NewTileInfo.PackedTileIndex = TileIndex + 384;
		UE_LOG(LogTemp, Warning, TEXT("RIGHT damaged. Old tile index: %i. New index: %i."), TileIndex, NewTileInfo.GetTileIndex());
	}

}
else //Rows with damaged tiles
{
	//Destroy tile
	NewTileInfo.PackedTileIndex = 0;
}

TileMapComponent->SetTile(TileX, TileY, 0, NewTileInfo);
TileMapComponent->RebuildCollision();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		return;
	}

	auto MyOwnerInstigator = MyOwner->GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
		Destroy();
	}

	FVector HitLocation = Hit.ImpactPoint;
	CurrentLevelTileMap = TanksGameMode->GetCurrentLevelTileMap();
	if (IsValid(CurrentLevelTileMap))
	{
		TileMapComponent = CurrentLevelTileMap->GetRenderComponent();
	}

	if (TileMapComponent && (Hit.Component == TileMapComponent))
	{
		FVector DiffVector = Hit.TraceEnd - Hit.TraceStart;
		FVector Direction = DiffVector.GetSafeNormal();
		FVector LocalHitLocation = TileMapComponent->GetComponentTransform().InverseTransformPosition(HitLocation + (4 * Direction));
		UPaperTileMap* TileMap = TileMapComponent->TileMap;

		// Get tile coordinates from local space position
		int32 TileX, TileY;
		TileMap->GetTileCoordinatesFromLocalSpacePosition(LocalHitLocation, TileX, TileY);

		DamageTile(Direction, TileX, TileY);

		//////	@TO DO
		//Search for a second tile in blast radius and damage it
		/*

		if (Direction.X != 0)
		{
			int32 LTileX, LTileY;
			int32 RTileX, RTileY;

			TileMap->GetTileCoordinatesFromLocalSpacePosition(LocalHitLocation + FVector(0, BlastRadiusY, 0), LTileX, LTileY);
			TileMap->GetTileCoordinatesFromLocalSpacePosition(LocalHitLocation + FVector(0, -BlastRadiusY, 0), RTileX, RTileY);

			if ((TileX != LTileX) &&)
			{

			}
		}
		if (Direction.Y != 0)
		{
			int32 LTileX, LTileY;
			int32 RTileX, RTileY;

			TileMap->GetTileCoordinatesFromLocalSpacePosition(LocalHitLocation + FVector(BlastRadiusX, 0, 0), LTileX, LTileY);
			TileMap->GetTileCoordinatesFromLocalSpacePosition(LocalHitLocation + FVector(-BlastRadiusX, 0, 0), RTileX, RTileY);
		}

		*/
	}
}

