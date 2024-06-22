// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

class AEnemyGroup;
class APaperHero;


/**
 * 
 */
UCLASS()
class LOSTANDFOUND_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()

public:

	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* PlayerDetection = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsSeeingPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsPlayerNear = false;

	UPROPERTY(VisibleAnywhere)
	bool bCanHitPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APaperHero* Player = nullptr;

	UPROPERTY(BlueprintReadWrite)
	class UCharacterMovementComponent* MovementPtr = GetCharacterMovement();

public:
	UFUNCTION(BlueprintCallable)
	void DoAction(FString str);

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void Charge();

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
	
};
