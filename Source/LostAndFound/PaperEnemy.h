// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PaperEnemy.generated.h"

class AEnemyGroup;
class APaperHero;
class UPaperFlipbook;
class USphereComponent;

/**
 * 
 */
UCLASS()
class LOSTANDFOUND_API APaperEnemy : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:

	APaperEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AEnemyGroup* OwningGroup = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* PlayerDetection = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UPaperFlipbookComponent* DamageFB = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsBusy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsPlayerNear = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsHitting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsPlayingAnger = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsPlayingQuestion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APaperHero* Player = nullptr;

	UPROPERTY(BlueprintReadWrite)
	class UCharacterMovementComponent* MovementPtr = GetCharacterMovement();

	UPROPERTY(EditDefaultsOnly)
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly)
	float Health = 30.f;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* HitEffect = nullptr;


	// ====== TIMERS ========
	FTimerHandle DeathTimer;

	FTimerHandle AngerTimer;
	float AngerTime = 1.5f;

	FTimerHandle QuestionTimer;
	float QuestionTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FTimerHandle HitTimerHandle;
	float HitTime = 0.7f;


public:

	UFUNCTION(BlueprintCallable)
	void DoAction();

	UFUNCTION(BlueprintCallable)
	void AttackBegin();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChasePawn(APawn* Pawn);
	void ChasePawn_Implementation(APawn* Pawn);

	UFUNCTION(BlueprintCallable)
	void PlayAngerAnimBegin();

	UFUNCTION(BlueprintCallable)
	void PlayAngerAnimEnd();

	UFUNCTION(BlueprintCallable)
	void PlayQuestionAnimBegin();

	UFUNCTION(BlueprintCallable)
	void PlayQuestionAnimEnd();

	UFUNCTION()
	void DetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void DetectionEndOverlap(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void GetHit(APaperHero* Hero, float ReceivedDamage);

	UFUNCTION()
	void DisableAll();

	UFUNCTION()
	void Die();

	UFUNCTION(BlueprintCallable)
	void PlayDamageEffect(UPaperFlipbook* NewDamageFB);
};
