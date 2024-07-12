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
class UDialogueWave;

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
	UInputAction* HitAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedCoef = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BlockWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector Direction = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bIsHitting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bIsBlocking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bCanBlock = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float HitTime = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float BlockTime = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* HitEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* BlockEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDialogueWave* HitSound;

	UPROPERTY(EditDefaultsOnly)
	UDialogueWave* ShieldSound;

	UPROPERTY(EditDefaultsOnly)
	UDialogueWave* DamageSound;


	// ====== TIMERS ========
	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float HitTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float BlockTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float BlockCooldown = BlockTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float BlockSeconds = BlockTime;

public:
	APaperHero();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Hit(const FInputActionValue& Value);

	UFUNCTION()
	void HitTick(float DeltaTime);

	UFUNCTION()
	void Block(const FInputActionValue& Value);

	UFUNCTION()
	void BlockReleased(const FInputActionValue& Value);

	UFUNCTION()
	void BlockTick(float DeltaTime);

	UFUNCTION()
	void OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void GetHit(APaperEnemy* Enemy, int ReceivedDamage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateHealthBar();
	void UpdateHealthBar_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateShieldBar();
	void UpdateShieldBar_Implementation();

	UFUNCTION(BlueprintCallable)
	void PlayDamageEffect(UPaperFlipbook* NewDamageFB);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Die();
	void Die_Implementation();

	// ============ SOUND NATIVE EVENTS ==============
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayHitSound();
	void PlayHitSound_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayShieldSound();
	void PlayShieldSound_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayDamageSound();
	void PlayDamageSound_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayGameOverSound();
	void PlayGameOverSound_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetAvatarDead();
	void SetAvatarDead_Implementation();
};
