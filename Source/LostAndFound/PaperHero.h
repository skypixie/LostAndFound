// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PaperHero.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UPaperFlipbookComponent;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class LOSTANDFOUND_API APaperHero : public APaperZDCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UPaperFlipbookComponent* DamageFB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UBoxComponent* AttackCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* HitAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedCoef = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector Direction = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bIsHitting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float HitTime = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float HitTimer = 0.0f;

private:
	UPROPERTY()
	float Health = 100.f;

	UPROPERTY()
	float Damage = 15.f;

public:
	APaperHero();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Dodge(const FInputActionValue& Value);

	UFUNCTION()
	void Hit(const FInputActionValue& Value);

	UFUNCTION()
	void HitTick(float DeltaTime);

	UFUNCTION()
	void OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackCollisionHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void GetHit(AActor* OtherActor, float ReceivedDamage);

	UFUNCTION(BlueprintCallable)
	void PlayDamageEffect(UPaperFlipbook* NewDamageFB);

	UFUNCTION(BlueprintCallable)
	void Die();
	
};
