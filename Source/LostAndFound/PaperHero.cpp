// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperHero.h"

#include "PaperEnemy.h"

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/DialogueWave.h"
#include "Sound/DialogueTypes.h"


APaperHero::APaperHero()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetSprite());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	DamageFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Damage FB"));
	DamageFB->SetupAttachment(GetSprite());
	DamageFB->SetLooping(false);

	AttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Box"));
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	AttackCollision->SetupAttachment(GetSprite());
	
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &APaperHero::OnAttackCollisionOverlap);

	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
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
	Input->BindAction(BlockAction, ETriggerEvent::Triggered, this, &APaperHero::Block);
	Input->BindAction(BlockAction, ETriggerEvent::Completed, this, &APaperHero::BlockReleased);
}

void APaperHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HitTick(DeltaTime);
	BlockTick(DeltaTime);
}

void APaperHero::Move(const FInputActionValue& Value)
{
	if (bIsHitting || bIsDead) return;
	Direction = Value.Get<FVector>();
	Direction.Normalize();
	
	// moves very slow without it
	AddActorWorldOffset(Direction * SpeedCoef, true, nullptr);
	AddMovementInput(Direction, SpeedCoef, true);

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("%f %f %f"), Direction.X, Direction.Y, Direction.Z));
}

void APaperHero::Hit(const FInputActionValue& Value)
{
	if (bIsBlocking) return;

	bIsHitting = true;

	// change collision location
	// x - left/right
	// y - up/down
	FVector FinalBoxLocation;
	FVector LastMove = GetLastMovementInputVector();
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("%f %f %f"), LastMove.X, LastMove.Y, LastMove.Z));

	// not moving, then use Direction
	if (!LastMove.Length()) LastMove = Direction;

	// moving vertically
	if (LastMove.X)
	{
		if (LastMove.X > 0.f) FinalBoxLocation = FVector(0.f, -40.f, -40.f);
		else FinalBoxLocation = FVector(0.f, 40.f, -40.f);
	}
	// moving horizontally
	else if (LastMove.Y)
	{
		if (LastMove.Y > 0.f) FinalBoxLocation = FVector(40.f, 0.f, -40.f);
		else FinalBoxLocation = FVector(-40.f, 0.f, -40.f);
	}
	AttackCollision->SetRelativeLocation(FinalBoxLocation);

	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APaperHero::HitTick(float DeltaTime)
{
	if (bIsHitting)
	{
		if (HitTimer >= 0.1f && HitTimer <= 0.11) PlayHitSound();

		if (HitTimer >= HitTime)
		{
			bIsHitting = false;
			HitTimer = 0.0f;
			AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			HitTimer += DeltaTime;
		}
	}
}

void APaperHero::Block(const FInputActionValue& Value)
{
	if (!bCanBlock) return;

	bIsBlocking = true;
	GetCharacterMovement()->MaxWalkSpeed = BlockWalkSpeed;

	DamageFB->SetFlipbook(BlockEffect);
	DamageFB->SetLooping(true);
	DamageFB->Play();
}

void APaperHero::BlockReleased(const FInputActionValue& Value)
{
	bIsBlocking = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	DamageFB->SetFlipbook(nullptr);
	DamageFB->SetLooping(false);
}

void APaperHero::BlockTick(float DeltaTime)
{
	UpdateShieldBar();

	if (bIsBlocking)
	{
		if (BlockTimer >= 0.1 && BlockTimer <= 0.11) PlayShieldSound();
		if (BlockTimer >= BlockTime)
		{
			bIsBlocking = false;
			bCanBlock = false;
			DamageFB->SetFlipbook(nullptr);
			DamageFB->SetLooping(false);
		}
		else
		{
			BlockTimer += DeltaTime;
		}
		BlockCooldown -= DeltaTime;
		BlockSeconds -= DeltaTime;

		return;
	}
	// cooldown
	if (BlockCooldown < BlockTime)
	{
		BlockCooldown += DeltaTime;
	}
	else if (BlockCooldown >= BlockTime)
	{
		bCanBlock = true;
		//BlockCooldown = 0.0f;
		BlockTimer = 0.0f;
	}
	BlockSeconds = BlockCooldown;
}

void APaperHero::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APaperEnemy* Enemy = Cast<APaperEnemy>(OtherActor);
	// because the enemy has a big collision sphere
	// we need to check if it collided with a root component which is a hitbox
	if (IsValid(Enemy) && (OtherActor->GetRootComponent() == OtherComp))
	{
		Enemy->GetHit(this, Damage);
	}
	// just take damage cuz its a boss ye ik im stupid
	else if (OtherActor->GetRootComponent() == OtherComp)
	{
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(Damage, DamageEvent, GetController(), this);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Boss Damage");
	}
}

void APaperHero::GetHit(APaperEnemy* Enemy, int ReceivedDamage)
{

	if (bIsBlocking) return;
	if (Health - ReceivedDamage <= 0)
	{
		Health = 0;
		UpdateHealthBar();
		Die();
	}
	else
	{
		Health -= ReceivedDamage;
		UpdateHealthBar();
		PlayDamageEffect(Enemy->HitEffect);
		PlayDamageSound();
		//FVector ImpulseDirection = GetActorLocation() - OtherActor->GetActorLocation();
		
		// too laggy
		//AddActorWorldOffset(ImpulseDirection, false);
		
		// doesnt work even if SimulatePhysics enabled
		//GetSprite()->AddImpulse(ImpulseDirection * 5, NAME_None, false);
		
		// Looks bad
		//LaunchCharacter(ImpulseDirection * 10, false, false);
	}
}

void APaperHero::UpdateHealthBar_Implementation()
{
}

void APaperHero::UpdateShieldBar_Implementation()
{
}

void APaperHero::PlayDamageEffect(UPaperFlipbook* NewDamageFB)
{
	DamageFB->SetFlipbook(NewDamageFB);
	DamageFB->PlayFromStart();
}

void APaperHero::Die_Implementation()
{
	if (bIsDead) return;

	DamageFB->SetFlipbook(nullptr);
	bIsDead = true;
	PlayGameOverSound();
	SetAvatarDead();
}

void APaperHero::PlayHitSound_Implementation()
{
}

void APaperHero::PlayShieldSound_Implementation()
{
}

void APaperHero::PlayDamageSound_Implementation()
{
}

void APaperHero::PlayGameOverSound_Implementation()
{
}

void APaperHero::SetAvatarDead_Implementation()
{
}
