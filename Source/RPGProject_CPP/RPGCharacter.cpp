// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Set Attributes: these will be default values that the characters inherit

	// Initial Health
	Health = 1.f;

	// Regen this much health per second
	HealthRegenAmount = 0.01f;

	// Initial Mana
	Mana = 1.f;

	// Regen this much mana per second
	ManaRegenAmount = 0.01f;

	// Initially abilities are off cooldown 
	Ability1Cooldown = 1.f;
	Ability2Cooldown = 1.f;

}

// Called when the game starts or when spawned
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(HUDWidget)
	{ 
		UUserWidget* RPG_HUD = CreateWidget<UUserWidget>(GetWorld(), HUDWidget, FName("RPG_HUD"));	
		if (RPG_HUD)
		{
			RPG_HUD->AddToViewport();
		}
	}
	
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When you multiply something with delta time that value becomes "units per second"

	HealthRegen(DeltaTime);

	ManaRegen(DeltaTime);

}

// Called to bind functionality to input
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARPGCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARPGCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARPGCharacter::OnResetVR);
}


void ARPGCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARPGCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ARPGCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ARPGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARPGCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ARPGCharacter::ResetCastingState(bool &bCastingState)
{
	// here we reset the casting state that was given to us
	bCastingState = false;

	// then we set the movement mode to walking to enable movement again
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	//UE_LOG(LogTemp, Warning, TEXT("bool timer run"));

	// clear the timer bc we only wanna run this function once
	//GetWorldTimerManager().ClearTimer(CastingStateTimerHandle);


}

void ARPGCharacter::ResetAbilityCooldown(float &AbilityCooldown)
{
	// keep adding to the cooldown at a rate of 0.05 per every 0.5 second (so 0.1 every 1 second), till it reaches 1
	if (AbilityCooldown < 1.f) 
	{
		AbilityCooldown += 0.05f;
	}
	else
	{
		//AbilityCooldown = 1f;
		// if it has reached 1 then we just clear this timer handle to stop it
		GetWorldTimerManager().ClearTimer(AbilityCooldownTimerHandle);
	}
}

void ARPGCharacter::HealthRegen(float DeltaTime)
{
	if (Health < 1.f)
	{
		ModifyHealth(DeltaTime * HealthRegenAmount);
	}
}

void ARPGCharacter::ManaRegen(float DeltaTime)
{
	if (Mana < 1.f)
	{
		ModifyMana(DeltaTime * ManaRegenAmount);
	}
}

void ARPGCharacter::NoManaPrompt()
{
	if(NoManaWidget)
	{ 
		UUserWidget* NoMana = CreateWidget<UUserWidget>(GetWorld(),NoManaWidget, FName("NoMana"));	
		if (NoMana)
		{
			NoMana->AddToViewport();
		}
	}
}

void ARPGCharacter::ModifyHealth(float Modifier)
{
	Health += Modifier;
	if (Health > 1.f)
	{
		Health = 1.f;
	}
	if (Health < 0.f)
	{
		Health = 0.f;
	}

}

void ARPGCharacter::ModifyMana(float Modifier)
{
	Mana += Modifier;
	if (Mana > 1.f)
	{
		Mana = 1.f;
	}
	if (Mana < 0.f)
	{
		Mana = 0.f;
	}
}

bool ARPGCharacter::IsHealthFull()
{
	if (Health == 1.f)
	{
		return true;
	}
	else 
	{
		return false;
	}
}
