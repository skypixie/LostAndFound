// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperHero.h"

#include "PaperEnemy.h"

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"


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
	//AttackCollision->OnComponentHit.AddDynamic(this, &APaperHero::OnAttackCollisionHit);
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
	
	// moves very slow without it
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

// isn't used for now cuz OnHit fits better
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
}

void APaperHero::OnAttackCollisionHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	APaperEnemy* Enemy = Cast<APaperEnemy>(OtherActor);
	if (IsValid(Enemy) && (OtherActor->GetRootComponent() == OtherComp))
	{
		Enemy->GetHit(this, Damage);
	}
}

void APaperHero::Interact(const FInputActionValue& Value)
{
}

void APaperHero::GetHit(AActor* OtherActor, float ReceivedDamage)
{
	if (Health - ReceivedDamage <= 0)
	{
		Die();
	}
	else
	{
		Health -= ReceivedDamage;

		APaperEnemy* Enemy = Cast<APaperEnemy>(OtherActor);
		if (Enemy)
		{
			PlayDamageEffect(Enemy->GetHitEffect());
		}
		//FVector ImpulseDirection = GetActorLocation() - OtherActor->GetActorLocation();
		// 
		// too laggy
		//AddActorWorldOffset(ImpulseDirection, false);
		// 
		// doesnt work even if SimulatePhysics enabled
		//GetSprite()->AddImpulse(ImpulseDirection * 5, NAME_None, false);
		// 
		// Looks bad
		//LaunchCharacter(ImpulseDirection * 10, false, false);
	}
}

void APaperHero::PlayDamageEffect(UPaperFlipbook* NewDamageFB)
{
	DamageFB->SetFlipbook(NewDamageFB);
	DamageFB->PlayFromStart();
}

void APaperHero::Die()
{
}
