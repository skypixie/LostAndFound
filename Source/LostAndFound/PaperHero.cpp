// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperHero.h"

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


APaperHero::APaperHero()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetSprite());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void APaperHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APaperHero::Move);
	Input->BindAction(HitAction, ETriggerEvent::Triggered, this, &APaperHero::Hit);
}

void APaperHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HitTick(DeltaTime);
}

void APaperHero::Move(const FInputActionValue& Value)
{
	Direction = Value.Get<FVector>();
	Direction.Normalize();
	AddActorWorldOffset(Direction * SpeedCoef, true, nullptr);
	AddMovementInput(Direction, SpeedCoef, true);

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("%f %f %f"), Direction.X, Direction.Y, Direction.Z));
}

void APaperHero::Dodge(const FInputActionValue& Value)
{
}

void APaperHero::Hit(const FInputActionValue& Value)
{
	bIsHitting = true;
}

void APaperHero::HitTick(float DeltaTime)
{
	if (bIsHitting)
	{
		if (HitTimer >= HitTime) {
			bIsHitting = false;
			HitTimer = 0.0f;
		}
		else
		{
			HitTimer += DeltaTime;
		}
	}
}

void APaperHero::Interact(const FInputActionValue& Value)
{
}
