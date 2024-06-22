// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyGroup.h"
#include "PaperHero.h"

#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AEnemy::AEnemy()
{
	PlayerDetection = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetection"));
	PlayerDetection->SetupAttachment(RootComponent);
	PlayerDetection->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerDetection->SetCollisionResponseToAllChannels(ECR_Overlap);
	PlayerDetection->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::SphereBeginOverlap);
	PlayerDetection->OnComponentEndOverlap.AddDynamic(this, &AEnemy::SphereEndOverlap);
}

// random action that depends on bIsPlayerNear var
void AEnemy::DoAction(FString str)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, str);

	if (bIsPlayerNear)
	{
		if (UKismetMathLibrary::RandomBool())
			Attack();
		else
			PlayAngerAnim();
	}
	else
	{
		if (UKismetMathLibrary::RandomBool())
			ChasePawn(Player);
		else
			PlayQuestionAnim();
	}
}

void AEnemy::Attack()
{
	/*
	[] проиграть анимацию на враге
	[] проиграть эффект атаки на игроке
	[] получение урона игроком
	*/
	// ВОЗМОЖНО ПЕРЕДЕЛАТЬ НА APaperZDCharacter !!!!
	// ТОГДА БУДЕТ УДОБНО РАБОТАТЬ С АНИМАЦИЯМИ
}

void AEnemy::Charge()
{
}

void AEnemy::ChasePawn_Implementation(APawn* Pawn)
{
}

void AEnemy::PlayAngerAnim()
{
}

void AEnemy::PlayQuestionAnim()
{
}

void AEnemy::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bIsSeeingPlayer && OtherActor == Player)
	{
		bIsPlayerNear = true;
		bIsSeeingPlayer = true;
		DoAction("Sphere Begin Overlap");
	}
}

void AEnemy::SphereEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == Player)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%p %p"), OtherActor, Player);
		bIsPlayerNear = false;
		DoAction("Sphere End Overlap");
	}
}
