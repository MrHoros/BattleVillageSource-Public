// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGW_TanksGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, Level);

/**
 * 
 */
UCLASS()
class TGW_TANKS_API ATGW_TanksGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	void ActorDied(AActor* DeadActor);
	class APaperTileMapActor* GetCurrentLevelTileMap();

	void KillPlayer();

	UFUNCTION(BlueprintPure)
	ATank* GetPlayerTank();

	UFUNCTION(BlueprintPure)
	int32 GetCurrentLevel();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameOver(bool bWonGame);

	UFUNCTION(BlueprintImplementableEvent)
	void OnNextLevel(int32 level);

	//UFUNCTION(BlueprintImplementableEvent)
	//void OnPlayerDied();

	UFUNCTION(BlueprintCallable)
	void HandleGameStart();

	void GoToLevel(int32 level);

private:
	float StartDelay = 3.f;
	int32 MaxLevels = 5;
	int32 CurrentLevel = 1;
	int32 CurrentEnemies = 0;

	int32 PlayerScore = 0;
	UPROPERTY(BlueprintAssignable, Category = "Event", meta = (AllowPrivateAccess = "true"))
	FOnLevelChanged OnLevelChanged;

	class ATanksPlayerController* TanksPlayerController;
	class APlayerCameraManager* CameraManager;
	class AEnemySpawner* EnemySpawner;
	ATank* Tank;
	AActor* CameraActor;
	FVector DefaultCameraActorLocation;
};
