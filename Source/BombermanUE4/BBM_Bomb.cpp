// Fill out your copyright notice in the Description page of Project Settings.


#include "BBM_Bomb.h"
#include "Engine/World.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "BBM_DestructibleObject.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ABBM_Bomb::ABBM_Bomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABBM_Bomb::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle ExplodeHandler;
	FTimerHandle TimeToDetectCollisionsHandler;
	GetComponents<UStaticMeshComponent>(MyMeshes);
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		World->GetTimerManager().SetTimer(ExplodeHandler, this, &ABBM_Bomb::Explode, TimeToExplode, false);
		//World->GetTimerManager().SetTimer(TimeToDetectCollisionsHandler, this, &ABBM_Bomb::EnableCollisionsAfterSpawn, TimeToDetectCollisions, false);
	}
	
}

// Called every frame
void ABBM_Bomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABBM_Bomb::SetBombColor_Implementation(FLinearColor Color)
{
	m_BombColor = Color;

	ApplyColorToBomb();
}

void ABBM_Bomb::ApplyColorToBomb()
{
	for (int32 i = 0; i < MyMeshes.Num(); i++)
	{
		m_DynamicMaterial = MyMeshes[i]->CreateAndSetMaterialInstanceDynamic(0);
		m_DynamicMaterial->SetVectorParameterValue(FName("Color"), m_BombColor);
	}
}

void ABBM_Bomb::Explode_Implementation()
{
	FVector ActorLocation = GetActorLocation();
	UWorld* World = GetWorld();

	TArray<FHitResult> VerticalOutHits;
	TArray<FHitResult> HorizontalOutHits;
	FVector VerticalBoxShape = FVector(50 + (ExplosionRange * 100), 50, 100);
	FCollisionShape VerticalCollisionBox = FCollisionShape::MakeBox(VerticalBoxShape);
	FVector HorizontalBoxShape = FVector(50, 50 + (ExplosionRange * 100), 100);
	FCollisionShape HorizontalCollisionBox = FCollisionShape::MakeBox(HorizontalBoxShape);

	bool bIsVerticalHit = World->SweepMultiByChannel(VerticalOutHits, ActorLocation, ActorLocation - FVector(0.0f, 0.0f, 1.0f), FQuat::Identity, ECC_WorldStatic, VerticalCollisionBox);
	bool bIsHorizontalHit = World->SweepMultiByChannel(HorizontalOutHits, ActorLocation, ActorLocation - FVector(0.0f, 0.0f, 1.0f), FQuat::Identity, ECC_WorldStatic, HorizontalCollisionBox);

	if (bIsVerticalHit)
	{
		for (auto& Hit : VerticalOutHits)
		{
			if (Hit.Actor->GetClass()->ImplementsInterface(UBBM_DestructibleObject::StaticClass()))
			{
				IBBM_DestructibleObject* DestructibleObject = Cast<IBBM_DestructibleObject>(Hit.Actor);
				DestructibleObject->DestroySelf();
			}
		}
	}

	if (bIsHorizontalHit)
	{
		for (auto& Hit : HorizontalOutHits)
		{
			if (Hit.Actor->GetClass()->ImplementsInterface(UBBM_DestructibleObject::StaticClass()))
			{
				IBBM_DestructibleObject* DestructibleObject = Cast<IBBM_DestructibleObject>(Hit.Actor);
				DestructibleObject->DestroySelf();
			}
		}
	}

	FActorSpawnParameters SpawnParams;
	TArray<FVector> SpawnPositions;
	SpawnPositions.Add(ActorLocation);

	for (int32 i = 1; i <= ExplosionRange; i++)
	{
		SpawnPositions.Add(FVector(ActorLocation.X + (100 * i), ActorLocation.Y, ActorLocation.Z));
		SpawnPositions.Add(FVector(ActorLocation.X - (100 * i), ActorLocation.Y, ActorLocation.Z));
		SpawnPositions.Add(FVector(ActorLocation.X, ActorLocation.Y + (100 * i), ActorLocation.Z));
		SpawnPositions.Add(FVector(ActorLocation.X, ActorLocation.Y - (100 * i), ActorLocation.Z));
	}

	for (int32 i = 0; i < SpawnPositions.Num(); i++)
	{
		World->SpawnActor<AActor>(BombExplosionFX, SpawnPositions[i], FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
	}

	BombExploded.Broadcast();
	Destroy();
}

void ABBM_Bomb::EnableCollisionsAfterSpawn_Implementation()
{
	SetActorEnableCollision(true);
}
