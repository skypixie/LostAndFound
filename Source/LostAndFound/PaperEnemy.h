// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PaperEnemy.generated.h"

class AEnemyGroup;
class APaperHero;
class UPaperFlipbook;

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
	class USphereComponent* PlayerDetection = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UPaperFlipbookComponent* DamageFB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsBusy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsPlayerNear = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bIsHitting = false;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FTimerHandle DamageEffectTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FTimerHandle HitTimerHandle;


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
	void PlayAngerAnim();

	UFUNCTION(BlueprintCallable)
	void PlayQuestionAnim();

	UFUNCTION()
	void SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void SphereEndOverlap(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void GetHit(APaperHero* Hero, float ReceivedDamage);

	UFUNCTION()
	void Die();

	UFUNCTION(BlueprintCallable)
	void PlayDamageEffect(UPaperFlipbook* NewDamageFB);
};
