// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperEnemy.h"


#include "EnemyGroup.h"
#include "PaperHero.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

APaperEnemy::APaperEnemy()
{
	DamageFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Damage FB"));
	DamageFB->SetupAttachment(GetSprite());
	DamageFB->SetLooping(false);

	PlayerDetection = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetection"));
	PlayerDetection->SetupAttachment(RootComponent);
	PlayerDetection->SetSphereRadius(200.f);
	PlayerDetection->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerDetection->SetCollisionResponseToAllChannels(ECR_Overlap);
	PlayerDetection->OnComponentBeginOverlap.AddDynamic(this, &APaperEnemy::DetectionBeginOverlap);
	PlayerDetection->OnComponentEndOverlap.AddDynamic(this, &APaperEnemy::DetectionEndOverlap);
}

void APaperEnemy::DoAction()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, str);

	if (!bIsDead && !bIsBusy)
	{
		if (bIsPlayerNear)
		{
			if (UKismetMathLibrary::RandomBool())
				AttackBegin();
			else
				PlayAngerAnimBegin();
		}
		else
		{
			if (UKismetMathLibrary::RandomBool())
				ChasePawn(Player);
			else
				PlayQuestionAnimBegin();
		}
	}
}

void APaperEnemy::AttackBegin()
{
	if (bIsHitting || bIsBusy) return;

	bIsHitting = true;
	bIsBusy = true;

	GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, this, &APaperEnemy::AttackEnd, HitTime, false);

	Player->GetHit(this, Damage);
}

void APaperEnemy::AttackEnd()
{
	bIsHitting = false;
	bIsBusy = false;

	GetWorld()->GetTimerManager().ClearTimer(HitTimerHandle);
	DoAction();
}

void APaperEnemy::ChasePawn_Implementation(APawn* Pawn)
{
	bIsBusy = true;
}

void APaperEnemy::PlayAngerAnimBegin()
{
	if (bIsBusy) return;

	bIsPlayingAnger = true;
	bIsBusy = true;
	GetWorld()->GetTimerManager().SetTimer(AngerTimer, this, &APaperEnemy::PlayAngerAnimEnd, AngerTime, false);
}

void APaperEnemy::PlayAngerAnimEnd()
{
	bIsPlayingAnger = false;
	bIsBusy = false;
	GetWorld()->GetTimerManager().ClearTimer(AngerTimer);
	
	if (bIsPlayerNear) AttackBegin();
	else DoAction();
}

void APaperEnemy::PlayQuestionAnimBegin()
{
	if (bIsBusy) return;

	bIsPlayingQuestion = true;
	bIsBusy = true;
	GetWorld()->GetTimerManager().SetTimer(QuestionTimer, this, &APaperEnemy::PlayQuestionAnimEnd, QuestionTime, false);
}

void APaperEnemy::PlayQuestionAnimEnd()
{
	bIsPlayingQuestion = false;
	bIsBusy = false;
	GetWorld()->GetTimerManager().ClearTimer(QuestionTimer);
	
	if (bIsPlayerNear) AttackBegin();
	else DoAction();
}

void APaperEnemy::DetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Player)
	{
		bIsPlayerNear = true;
		if (!bIsBusy)
		{
			DoAction();
		}
	}
}

void APaperEnemy::DetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == Player)
	{
		bIsPlayerNear = false;
		DoAction();
	}
}

void APaperEnemy::GetHit(APaperHero* Hero, float ReceivedDamage)
{
	if (Health - ReceivedDamage <= 0.f)
	{
		DisableAll();

		// float TimeToDeath = Hero->HitEffect->GetNumFrames() / Hero->HitEffect->GetFramesPerSecond();
		float TimeToDeath = 1.5f;
		GetWorld()->GetTimerManager().SetTimer(
			DeathTimer,
			this,
			&APaperEnemy::Die,
			TimeToDeath,
			false
		);
	}
	else
	{
		Health -= ReceivedDamage;
		PlayDamageEffect(Hero->HitEffect);
	}
}

void APaperEnemy::DisableAll()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// prevents pawn from moving
	MovementPtr->MaxWalkSpeed = 0.0f;
	PlayerDetection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	bIsBusy = false;
	bIsHitting = false;
	bIsPlayerNear = false;
	bIsDead = true;
}

void APaperEnemy::Die()
{
	OwningGroup->EnemyDestroyed(this);
	Destroy();
}

void APaperEnemy::PlayDamageEffect(UPaperFlipbook* NewDamageFB)
{
	DamageFB->SetFlipbook(NewDamageFB);
	DamageFB->PlayFromStart();
}
