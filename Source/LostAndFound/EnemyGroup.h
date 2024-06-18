// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "EnemyGroup.generated.h"

class UBoxComponent;
class AEnemy;
class APaperHero;

UCLASS()
class LOSTANDFOUND_API AEnemyGroup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyGroup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* SpawnBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	int StartNumEnemies = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurNumEnemies = StartNumEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AEnemy*> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (ExposeOnSpawn = true))
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APaperHero* Player = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnEnemies(int32 NumEnemies);

	UFUNCTION()
	void BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

};
