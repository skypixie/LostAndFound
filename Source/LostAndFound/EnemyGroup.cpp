// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGroup.h"

#include "Enemy.h"
#include "PaperHero.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyGroup::AEnemyGroup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Box"));
	SpawnBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyGroup::BoxBeginOverlap);
	RootComponent = SpawnBox;
}

// Called when the game starts or when spawned
void AEnemyGroup::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<APaperHero>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	SpawnEnemies(StartNumEnemies);
}

// Called every frame
void AEnemyGroup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyGroup::SpawnEnemies(int32 NumEnemies)
{
	for (int i = 0; i < NumEnemies; ++i)
	{
		FVector SpawnLocation = FMath::RandPointInBox(SpawnBox->GetNavigationBounds());
		FRotator SpawnRotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AEnemy* SpawnedEnemy = Cast<AEnemy>(GetWorld()->SpawnActor(EnemyClass, &SpawnLocation, &SpawnRotation, SpawnParameters));
		if (SpawnedEnemy)
		{
			SpawnedEnemy->OwningGroup = this;
			SpawnedEnemy->Player = Player;
			Enemies.Add(SpawnedEnemy);
		}
	}
}

void AEnemyGroup::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	for (auto Enemy : Enemies)
	{
		Enemy->bIsSeeingPlayer = true;
		Enemy->DoAction();
	}
}

