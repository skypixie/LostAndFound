// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperEnemy.h"


#include "EnemyGroup.h"
#include "PaperHero.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

APaperEnemy::APaperEnemy()
{
	DamageFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Damage FB"));
	DamageFB->SetupAttachment(GetSprite());
	DamageFB->SetLooping(false);

	PlayerDetection = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetection"));
	PlayerDetection->SetupAttachment(RootComponent);
	PlayerDetection->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerDetection->SetCollisionResponseToAllChannels(ECR_Overlap);
	PlayerDetection->SetSphereRadius(200.f);
	PlayerDetection->OnComponentBeginOverlap.AddDynamic(this, &APaperEnemy::SphereBeginOverlap);
	PlayerDetection->OnComponentEndOverlap.AddDynamic(this, &APaperEnemy::SphereEndOverlap);
}

void APaperEnemy::DoAction()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, str);

	bIsBusy = true;
	if (bIsPlayerNear)
	{
		if (UKismetMathLibrary::RandomBool())
			AttackBegin();
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

void APaperEnemy::AttackBegin()
{
	if (bIsHitting) return;

	bIsHitting = true;
	FLinearColor AttackColor = {255, 0, 0, 0.5};
	GetSprite()->SetSpriteColor(AttackColor);

	GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, this, &APaperEnemy::AttackEnd, 0.5f, false);

	Player->GetHit(this, Damage);

	/*
	[+] проиграть анимацию на враге
	[+] проиграть эффект атаки на игроке
	[+] получение урона игроком
	*/
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Start Hit");
}

void APaperEnemy::AttackEnd()
{
	bIsHitting = false;
	bIsBusy = false;

	FLinearColor NormalColor = { 1, 1, 1, 1 };
	GetSprite()->SetSpriteColor(NormalColor);

	GetWorld()->GetTimerManager().ClearTimer(HitTimerHandle);
	DoAction();
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stop Hit");
}

void APaperEnemy::ChasePawn_Implementation(APawn* Pawn)
{
}

void APaperEnemy::PlayAngerAnim()
{
	bIsBusy = false;
}

void APaperEnemy::PlayQuestionAnim()
{
	bIsBusy = false;
}

void APaperEnemy::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsBusy && OtherActor == Player)
	{
		bIsPlayerNear = true;
		DoAction();
	}
}

void APaperEnemy::SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == Player)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%p %p"), OtherActor, Player);
		bIsPlayerNear = false;
		DoAction();
	}
}

void APaperEnemy::GetHit(APaperHero* Hero, float ReceivedDamage)
{
	if (Health - ReceivedDamage <= 0.f)
	{
		Die();
	}
	else
	{
		Health -= ReceivedDamage;
		PlayDamageEffect(Hero->HitEffect);
	}
}

void APaperEnemy::Die()
{
	Destroy();
}

void APaperEnemy::PlayDamageEffect(UPaperFlipbook* NewDamageFB)
{
	float AnimLengthSeconds = NewDamageFB->GetNumFrames() / NewDamageFB->GetFramesPerSecond();
	GetWorld()->GetTimerManager().SetTimer(DamageEffectTimer, this,
		&APaperEnemy::PlayDamageEffectEnd, AnimLengthSeconds, false);
	
	DamageFB->SetVisibility(true);
	DamageFB->SetFlipbook(NewDamageFB);
	DamageFB->PlayFromStart();
}

void APaperEnemy::PlayDamageEffectEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(DamageEffectTimer);
	if (IsValid(DamageFB))
	{
		DamageFB->Stop();
		DamageFB->SetVisibility(false);
	}
}
