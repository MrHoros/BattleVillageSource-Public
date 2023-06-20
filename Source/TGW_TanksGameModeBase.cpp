// Copyright Epic Games, Inc. All Rights Reserved.


#include "TGW_TanksGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "EnemyTank.h"
#include "TanksPlayerController.h"
#include "PaperTileMapActor.h"
#include "GameFramework/PlayerState.h"
#include "TanksPlayerState.h"
#include "EnemySpawner.h"

void ATGW_TanksGameModeBase::ActorDied(AActor* DeadActor)
{
	ATanksPlayerState* TanksPlayerState = Cast<ATanksPlayerState>(Tank->GetPlayerState());

	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		TanksPlayerState->ChangeLives(-1);

		if (TanksPlayerController)
		{
			TanksPlayerController->SetPlayerEnabledState(false);
		}

		if (TanksPlayerState->GetLives() > 0)
		{
			HandleGameStart();
		}
		else
		{
			OnGameOver(true);
		}
		//OnPlayerDied();
		//OnGameOver(true);
	}
	else if (AEnemyTank* DestroyedEnemyTank = Cast<AEnemyTank>(DeadActor))
	{
		DestroyedEnemyTank->HandleDestruction();
		TanksPlayerState->ChangeScore(DestroyedEnemyTank->GetScoreReward());

		if (EnemySpawner->GetCurrentEnemies() == 0)
		{
			if (CurrentLevel < MaxLevels)
			{
				FTimerHandle DelayHandle;
				FTimerDelegate TimerDel;
				ATGW_TanksGameModeBase* that = this;
				TimerDel.BindLambda([that]()
					{
						that->GoToLevel(that->CurrentLevel + 1);
					});

				// Start the timer with a delay of 2 seconds
				GetWorld()->GetTimerManager().SetTimer(DelayHandle, TimerDel, 2.f, false);

			}
			else
			{
				OnGameOver(false);
			}
		}
	}
}

void ATGW_TanksGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	//HandleGameStart();
	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	TanksPlayerController = Cast<ATanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	CameraActor = CameraManager->GetViewTarget();

	if (CameraActor)
	{
		DefaultCameraActorLocation = CameraActor->GetActorLocation();
	}
}


void ATGW_TanksGameModeBase::HandleGameStart()
{
	OnStartGame();

	//GetCurrentLevelTileMap();
	EnemySpawner->SpawnEnemies();

	if (TanksPlayerController)
	{
		TanksPlayerController->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnableTimerHandle;
		FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(
			TanksPlayerController,
			&ATanksPlayerController::SetPlayerEnabledState,
			true
		);
		GetWorldTimerManager().SetTimer(PlayerEnableTimerHandle,
			PlayerEnableTimerDelegate,
			StartDelay,
			false
		);
	}
}

void ATGW_TanksGameModeBase::GoToLevel(int32 NewLevel)
{
	OnLevelChanged.Broadcast(NewLevel);
	OnNextLevel(NewLevel);
	FVector NewLocation = DefaultCameraActorLocation;
	NewLocation.X += (NewLevel-1) * 500;
	CameraActor->SetActorLocation(NewLocation);
	CurrentLevel = NewLevel;
	HandleGameStart();
}

ATank* ATGW_TanksGameModeBase::GetPlayerTank()
{
	if (Tank)
	{
		return Tank;
	}
	return NULL;
}

void ATGW_TanksGameModeBase::KillPlayer()
{
	auto TankInstigator = Tank->GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();

	UGameplayStatics::ApplyDamage(Tank, 99999, TankInstigator, this, DamageTypeClass);
}

int32 ATGW_TanksGameModeBase::GetCurrentLevel()
{
	return CurrentLevel;
}

APaperTileMapActor* ATGW_TanksGameModeBase::GetCurrentLevelTileMap()
{
	TArray<AActor*> TileMaps;
	UGameplayStatics::GetAllActorsOfClass(this, APaperTileMapActor::StaticClass(), TileMaps);

	FString MyTileMapName = FString::Printf(TEXT("PTM_Level%i"), CurrentLevel);

	for (auto TileMapsIt = TileMaps.CreateIterator(); TileMapsIt; TileMapsIt++)
	{
		AActor* Actor = *TileMapsIt;

		if ( Actor->GetName().Equals(MyTileMapName))
		{
			return Cast<APaperTileMapActor>(Actor);
		}
	}

	return nullptr;
}

