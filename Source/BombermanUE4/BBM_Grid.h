// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BBM_Grid.generated.h"

/**
 * 
 */

UENUM()
enum ActorType
{
	Wall,
	Floor,
	InsideWalls,
};

UCLASS()
class BOMBERMANUE4_API UBBM_Grid : public UObject
{
	GENERATED_BODY()
	
public:
	void InitializeGrid(int Width, int Height, float CellSize, TSubclassOf<AActor> FloorTile, TSubclassOf<AActor> FloorPlane, TSubclassOf<AActor> WallTile, TSubclassOf<AActor> InsideWallsTile, TSubclassOf<ACameraActor> CoolCameraClass);
	FTransform GetTransformFromGridReferenceCoordiantes(int x, int y);
	FVector GetGridCenterLocation();

protected:
	int32 _Width;
	int32 _Height;
	int8** Grid;
	TArray<AActor*> FloorActorCoordinates;
	TEnumAsByte<ActorType> ActorSpawnType;

	virtual void BeginDestroy() override;
};